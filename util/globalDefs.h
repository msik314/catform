#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

#ifndef NDEBUG
#include <stdint.h>
#include "util/assert.h"

#define CAT_MALLOC(size) catLCMalloc(size, __FILE__, __LINE__)
#define CAT_MEMALIGN(alignment, size) catLCMemalign(alignment, size, __FILE__, __LINE__)
#define CAT_REALLOC(ptr, size) catLCRealloc(ptr, size, __FILE__, __LINE__)
#define CAT_FREE(ptr) catLCFree(ptr)

#define CHECK_LEAKS() checkLeaks()
#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

void* catLCMalloc(uint32_t size, const char* fileName, uint32_t line);
void* catLCMemalign(uint32_t alignment, uint32_t size, const char* fileName, uint32_t line);
void* catLCRealloc(void* ptr, uint32_t size, const char* fileName, uint32_t line);
void catLCFree(void* ptr);

void checkLeaks();

#ifdef __cplusplus
};
#endif //__cplusplus

#else //NDEBUG

#include "rpmalloc/rpmalloc.h"

#define CHECK_LEAKS()

#define CAT_MALLOC(size) rpmalloc(size)
#define CAT_MEMALIGN(alignment, size) rpaligned_alloc(alignment, size)
#define CAT_REALLOC(ptr, size) rprealloc(ptr, size)
#define CAT_FREE(ptr) rpfree(ptr)

#endif //NDEBUG

#endif //GLOBAL_DEFS_H
