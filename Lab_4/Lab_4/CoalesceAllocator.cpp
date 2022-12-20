#include <exception>
#include <iostream>
#include <Windows.h>
#include "CoalesceAllocator.h"

CoalesceAllocator::CoalesceAllocator(size_t allocatorPageSize) :
	pageSize_(alignPageSize(allocatorPageSize)),
	page_(nullptr),
	cursor_(nullptr)
{}

CoalesceAllocator::~CoalesceAllocator()
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "CA is already destroyed or not initialized");
}

void CoalesceAllocator::init()
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "CA is already destroyed or not initialized");

	page_ = addNewPage(nullptr);
	cursor_ = setBlockHeaderAndFooter(page_->pPageStart, false, pageSize_ - sizeof(Page));
	addInFreeList(cursor_, cursor_, cursor_);

	isInitialized = true;
}

void CoalesceAllocator::destroy()
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "CA is already destroyed or not initialized");

	Page* selected = page_;
	do
	{
		Page* next = selected->pNextPage;
		VirtualFree(selected, 0, MEM_RELEASE);
		selected = next;
	} while (selected != page_);
	page_ = nullptr;

#if _DEBUG
	occupiedBlocks.clear();
#endif
	isDestroyed = true;
}

void* CoalesceAllocator::alloc(size_t contentSize)
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "CA is already destroyed or not initialized");

	size_t requiredBlockSize = (contentSize < kBlockMinSize ? kBlockMinSize : contentSize) + sizeof(BlockData) * 2;

	_ASSERT_EXPR(requiredBlockSize <= pageSize_ - sizeof(Page*), "Required block size is bigger than page size.");

	selectFirstFitFreeBlock(requiredBlockSize);
	BlockData* selectedHeader = cursor_;
	FreeListEntry* selectedFLEntry = getBlockFreeListEntry(cursor_);

	size_t remainSize = selectedHeader->size - requiredBlockSize;
	if (remainSize >= kBlockMinSize)
	{
		auto rawCursor = reinterpret_cast<byte*>(cursor_);

		rawCursor += requiredBlockSize;
		cursor_ = setBlockHeaderAndFooter(rawCursor, false, remainSize);

		if (selectedFLEntry->pNextFreeHeader == selectedHeader)
		{
			addInFreeList(cursor_, cursor_, cursor_);
		}
		else
		{
			addInFreeList(cursor_, selectedFLEntry->pPrevFreeHeader, selectedFLEntry->pNextFreeHeader);
		}
		setBlockHeaderAndFooter(reinterpret_cast<byte*>(selectedHeader), true, requiredBlockSize);
	}
	else
	{
		setBlockHeaderAndFooter(reinterpret_cast<byte*>(selectedHeader), true, selectedHeader->size);
		if (selectedFLEntry->pNextFreeHeader == selectedHeader)
		{
			page_ = addNewPage(page_);
			cursor_ = setBlockHeaderAndFooter(page_->pPageStart, true, pageSize_ - sizeof(Page));
			addInFreeList(cursor_, cursor_, cursor_);
		}
		else
		{
			removeFromFreeList(selectedFLEntry);
			cursor_ = selectedFLEntry->pNextFreeHeader;
		}
	}

#if _DEBUG
	occupiedBlocks.emplace(getBlockDataPtr(selectedHeader), selectedHeader->size);
	++totalAllocated;
#endif

	return getBlockDataPtr(selectedHeader);
}

bool CoalesceAllocator::free(void* p)
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "CA is already destroyed or not initialized");

	if (!isPtrInPages(p))
		return false;

	const auto blockStart = static_cast<byte*>(p) - sizeof(BlockData);
	auto header = reinterpret_cast<BlockData*>(blockStart);

	BlockData* prevHeader = nullptr;
	BlockData* nextHeader = nullptr;

	if (blockStart - sizeof(BlockData) > page_->pPageStart)
	{
		auto prevFooter = reinterpret_cast<BlockData*>(blockStart - sizeof(BlockData));
		if (!prevFooter->isBusy)
			prevHeader = reinterpret_cast<BlockData*>(blockStart - prevFooter->size);
	}

	if (blockStart + header->size < page_->pPageEnd)
	{
		nextHeader = reinterpret_cast<BlockData*>(blockStart + header->size);
		if (nextHeader->isBusy)
			nextHeader = nullptr;
	}

	if (prevHeader == nullptr && nextHeader == nullptr)
	{
		addInFreeList(header, cursor_, getBlockFreeListEntry(cursor_)->pNextFreeHeader);
		setBlockHeaderAndFooter(blockStart, false, header->size);
	}
	else
	{
		byte* finalBlockStart = prevHeader != nullptr ? reinterpret_cast<byte*>(prevHeader) : blockStart;
		size_t finalBlockSize = header->size +
			(prevHeader == nullptr ? 0 : prevHeader->size) +
			(nextHeader == nullptr ? 0 : nextHeader->size);

		FreeListEntry* flEntryToReplace;
		if (prevHeader != nullptr && nextHeader != nullptr)
		{
			removeFromFreeList(getBlockFreeListEntry(nextHeader));
			flEntryToReplace = getBlockFreeListEntry(prevHeader);
		}
		else if (prevHeader != nullptr)
		{
			flEntryToReplace = getBlockFreeListEntry(prevHeader);
		}
		else
		{
			flEntryToReplace = getBlockFreeListEntry(nextHeader);
		}

		header = setBlockHeaderAndFooter(finalBlockStart, false, finalBlockSize);
		if (flEntryToReplace->pPrevFreeHeader == flEntryToReplace->pNextFreeHeader)
		{
			addInFreeList(header, header, header);
		}
		else
		{
			addInFreeList(header, flEntryToReplace->pPrevFreeHeader, flEntryToReplace->pNextFreeHeader);
		}
	}
	cursor_ = header;

#if _DEBUG
	if(occupiedBlocks.erase(p)) std::cout << "Successfully released" << std::endl;
	++totalRelased;
#endif

	return true;
}

void CoalesceAllocator::selectFirstFitFreeBlock(size_t requiredSize)
{
	const BlockData* const start = cursor_;
	do
	{
		if (cursor_->size >= requiredSize)
			return;

		auto blockFreeMeta = getBlockFreeListEntry(cursor_);
		cursor_ = blockFreeMeta->pNextFreeHeader;
	} while (cursor_ != start);

	page_ = addNewPage(page_);
	BlockData* blockHeader = setBlockHeaderAndFooter(page_->pPageStart, false, pageSize_ - sizeof(Page));
	addInFreeList(blockHeader, cursor_, getBlockFreeListEntry(cursor_)->pNextFreeHeader);

	cursor_ = reinterpret_cast<BlockData*>(page_->pPageStart);
}


CoalesceAllocator::BlockData* CoalesceAllocator::setBlockHeaderAndFooter(byte* start, bool isBusy, size_t blockSize)
{
	auto header = reinterpret_cast<BlockData*>(start);
	header->isBusy = isBusy;
	header->size = blockSize;

	auto footer = reinterpret_cast<BlockData*>(start + blockSize - sizeof(BlockData));
	footer->isBusy = isBusy;
	footer->size = blockSize;
	return header;
}


void CoalesceAllocator::addInFreeList(BlockData* header, BlockData* prevFreeHeader, BlockData* nextFreeHeader)
{
	FreeListEntry* freeMeta = getBlockFreeListEntry(header);

	freeMeta->pPrevFreeHeader = prevFreeHeader;
	freeMeta->pNextFreeHeader = nextFreeHeader;

	if (prevFreeHeader != header) 
	{
		getBlockFreeListEntry(prevFreeHeader)->pNextFreeHeader = header;
		getBlockFreeListEntry(nextFreeHeader)->pPrevFreeHeader = header;
	}
}

void CoalesceAllocator::removeFromFreeList(const FreeListEntry* entry)
{
	getBlockFreeListEntry(entry->pPrevFreeHeader)->pNextFreeHeader = entry->pNextFreeHeader;
	getBlockFreeListEntry(entry->pNextFreeHeader)->pPrevFreeHeader = entry->pPrevFreeHeader;
}


byte* CoalesceAllocator::getBlockDataPtr(BlockData* header) const
{
	return reinterpret_cast<byte*>(header) + sizeof(BlockData);
}

CoalesceAllocator::FreeListEntry* CoalesceAllocator::getBlockFreeListEntry(BlockData* header) const
{
	return reinterpret_cast<FreeListEntry*>(getBlockDataPtr(header));
}


CoalesceAllocator::Page* CoalesceAllocator::addNewPage(Page* prevPage) const
{
	void* p = VirtualAlloc(nullptr, pageSize_, MEM_COMMIT, PAGE_READWRITE);
	if (p == nullptr)
		throw std::exception("Unable to reach allocated memory");

	byte* start = static_cast<byte*>(p) + sizeof(Page);
	byte* end = start + pageSize_ - sizeof(Page);

	auto page = static_cast<Page*>(p);
	new(page) Page(start, end);

	if (prevPage == nullptr)
	{
		page->pPrevPage = page;
		page->pNextPage = page;
	}
	else
	{
		page->pPrevPage = prevPage;
		page->pNextPage = prevPage->pNextPage;
		prevPage->pNextPage->pPrevPage = page;
		prevPage->pNextPage = page;
	}
	return page;
}

bool CoalesceAllocator::isPtrInPages(void* p)
{
	const Page* start = page_;
	do
	{
		if (page_->isPointerInPage(p))
			return true;
		page_ = page_->pNextPage;
	} while (page_ != start);
	return false;
}

size_t CoalesceAllocator::alignPageSize(size_t size)
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	size_t virtualPageSize = info.dwPageSize;
	return (size / virtualPageSize + (size % virtualPageSize > 0 ? 1 : 0)) * virtualPageSize;
}

bool CoalesceAllocator::Page::isPointerInPage(const void* p) const
{
	return p >= pPageStart && p < pPageEnd;
}


#if _DEBUG
void CoalesceAllocator::dumpStat(bool showPages) const
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "CA is already destroyed or not initialized");

	std::cout << "[Coalesce Allocator] Dump stats" << std::endl;
	if (showPages)
	{
		std::cout << "<Dump pages>" << std::endl;
	}

	size_t totalPages = 0;
	size_t freeBlocks = 0;
	
	size_t totalBlocks = 0;

	int pagesCount = 0;
	Page* page = page_;
	do
	{
		if(showPages)
		{
			std::cout << "Page " << static_cast<void*>(page->pPageStart) << " - " << static_cast<void*>(page->pPageEnd) << std::endl;
		}
		page = page->pNextPage;
		++pagesCount;
	} while (page != page_);

	std::cout << "Total free blocks: " << freeBlocks << "\tTotal occupied blocks: " << occupiedBlocks.size();
	std::cout << totalPages << " pages, " << pageSize_ << "b each, " << pageSize_ * totalPages << "b total" << std::endl;
	std::cout << "Total allocated: " << totalAllocated << "\tTotal released: " << totalRelased << std::endl;
	std::cout << std::endl;
}

void CoalesceAllocator::dumpBlocks() const
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "CA is already destroyed or not initialized");

	std::cout << "[Coalesce Allocator] Dump occupied blocks" << std::endl;
	for (auto& pair : occupiedBlocks)
	{
		std::cout << "Block at " << pair.first << " with size " << pair.second << std::endl;
	}
	std::cout << std::endl;
}

#endif