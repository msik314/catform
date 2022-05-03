#ifndef LINALLOC_H
#define LINALLOC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

void linInit(void* mem, uint32_t size);
void* linalign(uint32_t size, uint32_t alignment);
#define linalloc(size) linalign(size, 16)
void linReset();
void* linCleanup();

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //LINALLOC
