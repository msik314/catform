#include "util/linalloc.h"

#include <stdint.h>
#include <util/atomics.h>
#include <util/utilMacros.h>

typedef struct
{
    void* block;
    volatile uint64_t index;
    uint64_t end;
}
LinallocHeader;

static LinallocHeader allocData;

void linInit(void* mem, uint32_t size)
{
    allocData.block = mem;
    allocData.end = size + (uint64_t)allocData.block;
    linReset();
}

void* linalign(uint32_t size, uint32_t alignment)
{
    uint64_t index = atomicLoad64(&allocData.index);
    uint64_t ptr;
    uint64_t end;
    uint64_t align64 = alignment - 1;
    
    do
    {
        ptr = (index + align64) & (~align64);
        end = ptr + size;
        if(EXPECT(end >= allocData.end, 0)) return NULL;
    }
    while(!compareAndSwap64(&allocData.index, &index, end));
    
    return (void*)(ptr);
}

void linReset()
{
    atomicStore64(&allocData.index, (uint64_t)allocData.block);
}

void* linCleanup()
{
    return allocData.block;
}
