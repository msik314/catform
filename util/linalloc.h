#ifndef LINALLOC_H
#define LINALLOC_H

#include <stdint.h>

void linInit(void* mem, uint32_t size);
void* linalign(uint32_t size, uint32_t alignment);
#define linalloc(size) linalign(size, 16)
void linReset();
void* linCleanup();

#endif //LINALLOC
