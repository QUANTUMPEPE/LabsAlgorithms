#include "MemoryAllocator.h"
#include <windows.h>

MemoryAllocator::MemoryAllocator() :
	fsa16_(16),
	fsa32_(32),
	fsa64_(64),
	fsa128_(128),
	fsa256_(256),
	fsa512_(512),
	ca_(kCABlockMaxSize*100)
{
	_ASSERT_EXPR(!isDestroyed, "Memory allocator is already destroyed");
}

MemoryAllocator::~MemoryAllocator()
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "Memory allocator is already destroyed or not initialized");
}

void MemoryAllocator::init()
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "Memory allocator is already destroyed or not initialized");
	fsa16_.init();
	fsa32_.init();
	fsa64_.init();
	fsa128_.init();
	fsa256_.init();
	fsa512_.init();
	ca_.init();

#if _DEBUG
	isInitialized = true;
#endif
}

void MemoryAllocator::destroy()
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "Memory allocator is already destroyed or not initialized");
	fsa16_.destroy();
	fsa32_.destroy();
	fsa64_.destroy();
	fsa128_.destroy();
	fsa256_.destroy();
	fsa512_.destroy();
	ca_.destroy();

#if _DEBUG
	isDestroyed = true;
#endif
}

void* MemoryAllocator::alloc(size_t size)
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "Memory allocator is already destroyed or not initialized");

	void* data;

	if(size <= 16) data = fsa16_.alloc();
	else if(size <= 32) data = fsa32_.alloc();
	else if(size <= 64) data = fsa64_.alloc();
	else if(size <= 128) data = fsa128_.alloc();
	else if(size <= 256) data = fsa256_.alloc();
	else if(size <= 512) data = fsa512_.alloc();
	else if(size <= kCABlockMaxSize) data = ca_.alloc(size);
	else
	{
		data = VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
#ifdef _DEBUG	
		++osAllocated;
#endif // _DEBUG
	}
	return data;
}

void MemoryAllocator::free(void* p)
{
	if(fsa16_.free(p)) return;
	else if(fsa32_.free(p)) return;
	else if(fsa64_.free(p)) return;
	else if(fsa128_.free(p)) return;
	else if(fsa256_.free(p)) return;
	else if(fsa512_.free(p)) return;
	else if(ca_.free(p)) return;
	else{ VirtualFree(p, 0, MEM_RELEASE); }
}

#if _DEBUG
void MemoryAllocator::dumpStat(bool showPages) const
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "Memory allocator is already destroyed or not initialized");
	fsa16_.dumpStat(showPages);
	fsa32_.dumpStat(showPages);
	fsa64_.dumpStat(showPages);
	fsa128_.dumpStat(showPages);
	fsa256_.dumpStat(showPages);
	fsa512_.dumpStat(showPages);
	ca_.dumpStat(showPages);

}

void MemoryAllocator::dumpBlocks() const
{
	_ASSERT_EXPR(!isDestroyed || isInitialized, "Memory allocator is already destroyed or not initialized");
	fsa16_.dumpBlocks();
	fsa32_.dumpBlocks();
	fsa64_.dumpBlocks();
	fsa128_.dumpBlocks();
	fsa256_.dumpBlocks();
	fsa512_.dumpBlocks();
	ca_.dumpBlocks();
}
#endif