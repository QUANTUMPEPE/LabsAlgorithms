#pragma once
#include <map>

typedef unsigned char byte;

class FixedSizeAllocator
{

//Forward declaration
private:
	struct Page;


public:
	FixedSizeAllocator() = delete;

	FixedSizeAllocator(size_t blockSize, size_t pageBlocks = 1024) : 
		blockSize_(blockSize), pageBlocks_(pageBlocks){}

	virtual void init();
	virtual void destroy();

	virtual void* alloc ();
	virtual bool free (void *p);

	virtual ~FixedSizeAllocator();

protected:
	virtual void addNewPage();

private:

	struct Page
	{
		Page(size_t pageBlocks, byte* pPageStart, byte* pPageEnd);

		size_t pageBlocks;

		//byte* pNextFreeBlock;
		byte* pFreeListHead;
		byte* pPageStart;
		byte* pPageEnd;
		
		size_t occupiedBlocks;

		Page* pNextPage;

		inline bool isFull() { return occupiedBlocks == pageBlocks-1; }
		inline bool isBlockInPage (const byte* block) { return block > pPageStart && block <= pPageEnd; } 
		
	};

	Page* pFirstPage_;
	
	const size_t pageBlocks_;
	const size_t blockSize_;
	size_t pageSize_;

	bool isInitialized = false;
	bool isDestroyed = false;

	Page* findBlocksPage(const byte* block) const;

	inline byte* getPointerAt(Page* page, size_t index) { return page->pPageStart + sizeof(Page) + blockSize_ * index; }
	inline size_t getIndexAt(byte* p) { return *reinterpret_cast<size_t*>(p); }

#ifdef _DEBUG
public:
	virtual void dumpStat(bool showPages = false) const;
	virtual void dumpBlocks() const;
	std::map <void*, size_t> occupiedBlocks;
private:
	size_t totalAllocated = 0;
	size_t totalRelased = 0;
#endif // _DEBUG
};

