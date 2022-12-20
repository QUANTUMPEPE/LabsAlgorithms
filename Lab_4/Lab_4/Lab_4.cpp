#include <iostream>
#include "FixedSizeAllocator.h"
#include "CoalesceAllocator.h"
#include "MemoryAllocator.h"


int main()
{
	MemoryAllocator MA;
	MA.init();
	MA.alloc(8);
	MA.alloc(16);
	MA.alloc(32);
	MA.alloc(64);
	MA.alloc(128);
	MA.alloc(256);
	MA.alloc(512);
	MA.alloc(1024);
	MA.alloc(2048);
	MA.alloc(4096);
	MA.alloc(8192);
	MA.alloc(16384);
	MA.dumpBlocks();
	MA.dumpStat(true);
	MA.destroy();
}
