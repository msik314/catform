#include "util/globalDefs.h"
#include "util/leakCheck.h"

#ifndef NDEBUG

#include <stdio.h>
#include <unordered_map>
#include "util/assert.h"
#include "rpmalloc/rpmalloc.h"


std::unordered_map<void*, AllocData>& getTrackedPointers()
{
    static std::unordered_map<void*, AllocData> trackedPointers;
    return trackedPointers;
}

void* catLCMalloc(uint32_t size, const char* fileName, uint32_t line)
{
    void* mem = rpmalloc(size);
    
    getTrackedPointers()[mem] = {fileName, line};
    
    return mem;
}

void* catLCRealloc(void* ptr, uint32_t size, const char* fileName, uint32_t line)
{
    void* mem = rprealloc(ptr, size);
    
    if(ptr != mem)
    {
        if(ptr) getTrackedPointers().erase(ptr);
        if(mem) getTrackedPointers()[mem] = {fileName, line};
    }
    
    return mem;
}

void* catLCMemalign(uint32_t alignment, uint32_t size, const char* fileName, uint32_t line)
{
    void* mem = rpaligned_alloc(alignment, size);
    
    getTrackedPointers()[mem] = {fileName, line};
    
    return mem;
}

void catLCFree(void* ptr)
{
    size_t res = getTrackedPointers().erase(ptr);
    if(!res && ptr)
    {
        fprintf(stderr, "Pointer %x has not been allocated or has been freed twice\n", ptr);
        ASSERT(false, "Memory error detected");
    }
    rpfree(ptr);
}

void checkLeaks()
{
    if(getTrackedPointers().size() != 0)
    {
        std::unordered_map<void*, AllocData>::iterator itr = getTrackedPointers().begin();
        while(itr != getTrackedPointers().end())
        {
            fprintf(stderr, "Unfreed pointer %x allocated at %s:%d\n", itr->first, itr->second.fileName, itr->second.line);
            ++itr;
        }
        
        ASSERT(false, "Memory leak detected");
    }
}

#endif //NDEBUG
