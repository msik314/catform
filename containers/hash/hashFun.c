#include "containers/hash/hashFun.h"

#include <stdint.h>

#define C1 0xcc932d51
#define C2 0x1b873593
#define R1 15
#define R2 13
#define M 5
#define N 0xe6546b64

static inline uint32_t rotl (uint32_t x, uint32_t r)
{
  return (x << r) | (x >> (32 - r));
}

uint32_t murmur3(uint8_t* data, uint32_t len, uint32_t seed)
{
    uint32_t hash = seed;
    uint32_t key;
    
    for(uint32_t i = len >> 2; i; --i)
    {
        key = ((uint32_t*)data)[0];
        data += sizeof(uint32_t);
        
        key *= C1;
        key = rotl(key, R1);
        key *= C2;
        
        hash ^= key;
        hash = rotl(hash, R2);
        hash = (hash * M) + N;
    }
    
    key = 0;
    for(uint32_t left = len & 3;  left; --left)
    {
        key <<= 8;
        key |= data[left - 1];
    }
    
    key *= C1;
    key = rotl(key, R1);
    key *= C2;
    
    hash ^= key;
    hash ^= len;
    
    hash = hash ^ (hash >> 16);
    hash = hash * 0x85ebca6b;
    hash = hash ^ (hash >> 13);
    hash = hash * 0xc2b2ae35;
    hash = hash ^ (hash >> 16);
    
    return hash & 0x7fffffff;
}
