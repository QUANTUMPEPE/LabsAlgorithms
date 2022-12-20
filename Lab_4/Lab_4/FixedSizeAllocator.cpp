#include "FixedSizeAllocator.h"
#include "Windows.h"
#include <vcruntime_exception.h>
#include <intsafe.h>
#include <crtdbg.h>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

FixedSizeAllocator::~FixedSizeAllocator()
{
	_ASSERT_EXPR(isDestroyed, "FSA is not destroyed");
}

void FixedSizeAllocator::init()
{

	_ASSERT_EXPR(!isDestroyed || isInitialized, "FSA is already destroyed or not initialized");

	addNewPage();
	isInitialized = true;

	pageSize_ = blockSize_ * pageBlocks_ + sizeof(Page);
}

void FixedSizeAllocator::destroy()
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "FSA is already destroyed or not initialized");

	Page* nextPage = pFirstPage_;
	do
	{
		Page* tempPage = nextPage->pNextPage;
		VirtualFree(nextPage, 0, MEM_RELEASE);
		nextPage = tempPage;
	} while (nextPage != nullptr);
	pFirstPage_ = nullptr;

	totalAllocated = 0;
	totalRelased = 0;
	isDestroyed = true;
}

void* FixedSizeAllocator::alloc()
{
	_ASSERT_EXPR(isInitialized, "FSA is not initialized");

	Page* pageToInsert = pFirstPage_;
	while (pageToInsert->pNextPage && pageToInsert->isFull())
	{
		pageToInsert = pageToInsert->pNextPage;
	}
	if (pageToInsert->isFull())
	{
		addNewPage();
		pageToInsert = pageToInsert->pNextPage;
	}
	
	void* block = pageToInsert->pFreeListHead;
	const size_t index = getIndexAt(pageToInsert->pFreeListHead);
	pageToInsert->pFreeListHead = getPointerAt(pageToInsert, index);
	++(pageToInsert->occupiedBlocks); //Do I really need ()?

#ifdef _DEBUG
	++totalAllocated;
	occupiedBlocks.emplace(block, index);
#endif // _DEBUG
	
	return block;
}

bool FixedSizeAllocator::free(void* p)
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "FSA is already destroyed or not initialized");

	auto block = static_cast<byte*>(p);
	Page* pageToRemoveFrom = findBlocksPage(block);
	if(!pageToRemoveFrom)
	{
		_ASSERT_EXPR(pageToRemoveFrom, "Block %s was not allocated by this FSA", *to_string(p));
		return false;
	}
	
	*reinterpret_cast<size_t*>(block) = pageToRemoveFrom->isFull() ? SIZE_T_MAX : getIndexAt(pageToRemoveFrom->pFreeListHead);
	pageToRemoveFrom->pFreeListHead = block;
	--(pageToRemoveFrom->occupiedBlocks); //Do I really need ()?

#ifdef _DEBUG
	++totalRelased;
	occupiedBlocks.erase(p);
#endif // _DEBUG

	return true;
}

void FixedSizeAllocator::addNewPage()
{
	const size_t pageSize = pageBlocks_ * blockSize_ + sizeof(Page);
	void* rawAllocMem = VirtualAlloc(nullptr, pageSize, MEM_COMMIT, PAGE_READWRITE);
	if(!rawAllocMem) throw std::exception("Unable to reach allocated memory");

	auto newPage = static_cast<Page*>(rawAllocMem);
	newPage = new Page(pageBlocks_, static_cast<byte*>(rawAllocMem), static_cast<byte*>(rawAllocMem) + pageSize); //TODO: check end address

	//Free blocks indexes 
	for(size_t i = 0; i < pageBlocks_-1; ++i)
		*reinterpret_cast<size_t*>(getPointerAt(newPage, i)) = i + 1;
	*reinterpret_cast<size_t*>(getPointerAt(newPage, pageBlocks_-1)) = SIZE_T_MAX;

	if(pFirstPage_)	//Linking new page to last page
	{	
		Page* nextPage = pFirstPage_;
		while (nextPage->pNextPage) {nextPage = nextPage->pNextPage;}

		nextPage->pNextPage = newPage;
	}
	else
	{
		pFirstPage_ = newPage;
	} 
}

FixedSizeAllocator::Page* FixedSizeAllocator::findBlocksPage(const byte* block) const
{
	Page* nextPage = pFirstPage_;
	do
	{
		if (nextPage->isBlockInPage(block))
			return nextPage;
		nextPage = nextPage->pNextPage;
	} while (nextPage != nullptr);
	return nullptr;
}

#ifdef _DEBUG
void FixedSizeAllocator::dumpStat(bool showPages) const
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "FSA is already destroyed or not initialized");

	std::cout << "[Fixed Size Allocator " << blockSize_ << "] Dump stats" << std::endl;
	// Count of occupied and free blocks
	if(showPages)
	{
		std::cout << "<Dump pages>" << std::endl;
	}
	
	size_t pagesCount = 0;
	size_t freeBlocks = 0;
	size_t occupiedBlocks = 0;
	size_t totalBlocks = 0;

	Page* page = pFirstPage_;
	do
	{
		freeBlocks += page->pageBlocks - page->occupiedBlocks;
		occupiedBlocks += page->occupiedBlocks;
		totalBlocks += page->pageBlocks;

		auto rawPtrPage = reinterpret_cast<byte*>(page);
		if(showPages)
			std::cout << "Page " << static_cast<void*>(rawPtrPage) << " - " << static_cast<void*>(rawPtrPage + pageSize_) << std::endl;
		page = page->pNextPage;
		++pagesCount;
	} while (page != nullptr);

	std::cout << "Total free blocks: " << freeBlocks << "\tTotal occupied blocks: " << occupiedBlocks << "\tTotal blocks: " << totalBlocks << std::endl;
	std::cout << pagesCount << " pages, " << pageSize_ << "b each, " << pageSize_ * pagesCount << "b total" << std::endl;
	std::cout << "Total allocated: " << totalAllocated << "\tTotal released: " << totalRelased << std::endl;
	std::cout << std::endl;
}

void FixedSizeAllocator::dumpBlocks() const
{
	std::cout << "[Fixed Size Allocator " << blockSize_ << "] Dump occupied blocks" << std::endl;
	for(auto& pair : occupiedBlocks)
	{
		std::cout << "Block index " << pair.second << " at " << pair.first << std::endl;
	}
	std::cout << std::endl;
}

#endif // _DEBUG

FixedSizeAllocator::Page::Page(size_t pageBlocks, byte* pPageStart, byte* pPageEnd)
{
	this->occupiedBlocks = 0;
	this->pageBlocks = pageBlocks;
	this->pPageStart = pPageStart;
	this->pPageEnd = pPageEnd;
	pNextPage = nullptr;
	pFreeListHead = pPageStart + sizeof(Page);
}

