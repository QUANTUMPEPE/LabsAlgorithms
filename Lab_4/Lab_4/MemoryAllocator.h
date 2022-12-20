#pragma once
#include "FixedSizeAllocator.h"
#include "CoalesceAllocator.h"
class MemoryAllocator
{
public:
	MemoryAllocator();
	virtual ~MemoryAllocator();

	virtual void init();
	virtual void destroy();

	virtual void* alloc(size_t size);
	virtual void free(void* p);

	const size_t kCABlockMaxSize = 10240;

private:
	FixedSizeAllocator fsa16_;
	FixedSizeAllocator fsa32_;
	FixedSizeAllocator fsa64_;
	FixedSizeAllocator fsa128_;
	FixedSizeAllocator fsa256_;
	FixedSizeAllocator fsa512_;
	CoalesceAllocator ca_;

	bool isInitialized = false;
	bool isDestroyed = false;

#if _DEBUG
public:
	virtual void dumpStat(bool showPages = false) const;
	virtual void dumpBlocks() const;
private:
	size_t osAllocated = 0;
#endif
};

