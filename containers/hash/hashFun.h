#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

#include <stdint.h>

#define hashData(data) murmur3((uint8_t*)(&(data)), sizeof(data), 0)
#define hashPtr(ptr, len) murmur3((uint8_t*)(ptr), len, 0)


#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
    
uint32_t murmur3(uint8_t* data, uint32_t len, uint32_t seed);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //HASH_FUNCTION_H
