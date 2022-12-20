#pragma once
#include "CoalesceAllocator.h"
#include <map>

typedef unsigned char byte;

class CoalesceAllocator
{
public:
	CoalesceAllocator(size_t allocatorPageSize = 10240);

	~CoalesceAllocator();

	CoalesceAllocator() = delete;

	void init();
	void destroy();

	void* alloc(size_t dataSize);
	bool free(void* p);

public:
	const size_t kBlockMinSize = sizeof(BlockData) * 2 + sizeof(FreeListEntry);
	const size_t kBlockDataSize = sizeof(Page);

private:
	struct Page;
	struct BlockData;
	struct FreeListEntry;

	const size_t pageSize_;

	Page* page_; 
	BlockData* cursor_; 

	void selectFirstFitFreeBlock(size_t requiredSize);
	BlockData* setBlockHeaderAndFooter(byte* start, bool isBusy, size_t blockSize);
	void addInFreeList(BlockData* header, BlockData* prevFreeHeader, BlockData* nextFreeHeader);
	void removeFromFreeList(const FreeListEntry* entry);

	byte* getBlockDataPtr(BlockData* header) const;
	FreeListEntry* getBlockFreeListEntry(BlockData* header) const;

	Page* addNewPage(Page* prevPage) const;

	bool isPtrInPages(void* p);

	size_t alignPageSize (size_t size);

	bool isInitialized = false;
	bool isDestroyed = false;

	struct Page
	{
		byte* pPageStart;
		byte* pPageEnd;
		Page* pPrevPage;
		Page* pNextPage;

		Page(byte* pageStart, byte* pageEnd)
			: pPageStart(pageStart), pPageEnd(pageEnd),
			pPrevPage(nullptr), pNextPage(nullptr)
		{}

		bool isPointerInPage(const void* p) const;
	};

	struct BlockData
	{
		bool isBusy;
		size_t size; // including header + footer

		BlockData(bool isBusy, size_t size) : isBusy(isBusy), size(size) { }
	};

	struct FreeListEntry
	{
		BlockData* pPrevFreeHeader;
		BlockData* pNextFreeHeader;
	};

#if _DEBUG
public:
	void dumpStat(bool showPages = false) const;
	void dumpBlocks() const;

private:
	size_t totalAllocated = 0;
	size_t totalRelased = 0;

	std::map<void*, size_t> occupiedBlocks;

#endif
};
