#include "util/globalDefs.h"
#include "containers/hashmap.h"

#include <stdint.h>
#include "containers/hash/hashFun.h"

#ifndef CAT_MALLOC
#include <stdlib.h>
#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

#define EMPTY_HASH 0x00000000
#define INVALID_HASH 0x80000000
#define STARTING_SIZE 8

static inline uint32_t probeDistance(uint32_t pos, uint32_t hash, uint32_t capacityMask)
{
    if(hash == EMPTY_HASH) return 0;
    hash &= capacityMask;
    return (pos - hash) & capacityMask;
}

#define hashmapInit(K, V) hashmapInit ## K ## V
#define hashmapIndex(K, V) hashmapIndex ## K ## V
#define hashmapInsert(K, V) hashmapInsert ## K ## V
#define hashmapResize(K, V) hashmapResize ## K ## V

#define HASHMAP_IMPL_EQ(K, V, EQ)\
static inline void hashmapInit(K, V)(Hashmap(K, V)* map, uint32_t capacity)\
{\
    map->map = CAT_MALLOC(capacity * sizeof(HashmapLine(K, V)));\
    for(uint32_t i = 0; i < capacity; ++i)\
    {\
        map->map[i].hash = EMPTY_HASH;\
    }\
    map->capacity = capacity;\
    map->size = 0;\
}\
\
void hashmapCreate(K, V)(Hashmap(K, V)* map)\
{\
    hashmapInit(K, V)(map, STARTING_SIZE);\
}\
\
void hashmapDestroy(K, V)(Hashmap(K, V)* map)\
{\
    CAT_FREE(map->map);\
}\
\
static inline int32_t hashmapIndex(K,V)(const Hashmap(K, V)* map, const K* key)\
{\
    uint32_t hash = hashPtr(key, sizeof(K));\
    uint32_t mask = (map->capacity - 1);\
    uint32_t pos = hash & mask;\
    uint32_t distance = 0;\
    \
    while(distance < map->capacity)\
    {\
        if(map->map[pos].hash == EMPTY_HASH) return -1;\
        if(distance > probeDistance(pos, map->map[pos].hash, mask)) return -1;\
        if(map->map[pos].hash == hash && EQ(&map->map[pos].key, key)) return pos;\
        pos = (pos + 1) & mask;\
        ++distance;\
    }\
    \
    return -1;\
}\
\
int32_t hashmapGet(K, V)(const Hashmap(K, V)* map, const K* key, V* outValue)\
{\
    int32_t index = hashmapIndex(K, V)(map, key);\
    if(index >= 0)\
    {\
        *outValue = map->map[index].value;\
        return 1;\
    }\
    return 0;\
}\
\
static inline int32_t hashmapInsert(K, V)(Hashmap(K, V)* map, uint32_t* hash, K* key, V* value)\
{\
    uint32_t mask = (map->capacity - 1);\
    uint32_t pos = *hash & mask;\
    uint32_t distance = 0;\
    uint32_t existingDistance;\
    K tmpKey;\
    V tmpValue;\
    uint32_t tmpHash;\
    \
    while(distance < map->capacity)\
    {\
        if(map->map[pos].hash == EMPTY_HASH)\
        {\
            map->map[pos].hash = *hash;\
            map->map[pos].key = *key;\
            map->map[pos].value = *value;\
            \
            ++(map->size);\
            \
            return 1;\
        }\
        \
        if(map->map[pos].hash == *hash && EQ(&map->map[pos].key, key))\
        {\
            map->map[pos].value = *value;\
            return 1;\
        }\
        existingDistance = probeDistance(pos, map->map[pos].hash, mask);\
        \
        if(distance > existingDistance)\
        {\
            if(map->map[pos].hash & INVALID_HASH)\
            {\
                map->map[pos].hash = *hash;\
                map->map[pos].key = *key;\
                map->map[pos].value = *value;\
                \
                ++(map->size);\
                \
                return 1;\
            }\
            \
            tmpHash = map->map[pos].hash;\
            tmpKey = map->map[pos].key;\
            tmpValue = map->map[pos].value;\
            \
            map->map[pos].hash = *hash;\
            map->map[pos].key = *key;\
            map->map[pos].value = *value;\
            \
            *hash = tmpHash;\
            *key = tmpKey;\
            *value = tmpValue;\
            distance = existingDistance;\
        }\
        \
        pos = (pos + 1) & mask;\
        ++distance;\
    }\
    \
    return 0;\
}\
\
static inline void hashmapResize(K, V)(Hashmap(K, V)* map)\
{\
    Hashmap(K, V) tmpMap;\
    hashmapInit(K, V)(&tmpMap, map->capacity << 1);\
    HashmapLine(K, V)* tmpBuffer;\
    \
    for(uint32_t i = 0; i < map->capacity; ++i)\
    {\
        if(map->map[i].hash != EMPTY_HASH && (map->map[i].hash & INVALID_HASH) == 0)\
        {\
            hashmapInsert(K, V)(&tmpMap, &(map->map[i].hash), &(map->map[i].key), &(map->map[i].value));\
        }\
    }\
    tmpBuffer = map->map;\
    *map = tmpMap;\
    CAT_FREE(tmpBuffer);\
}\
\
void hashmapSet(K, V)(Hashmap(K, V)* map, const K* key, const V* value)\
{\
    uint32_t hash = hashPtr(key, sizeof(K));\
    K tmpKey = *key;\
    V tmpValue = *value;\
    \
    while(1)\
    {\
       if(hashmapInsert(K, V)(map, &hash, &tmpKey, &tmpValue)) break;\
       hashmapResize(K, V)(map);\
    }\
    \
    if((map->size << 2) > 3 * map->capacity) hashmapResize(K, V)(map);\
}\
\
void hashmapRemove(K, V)(Hashmap(K, V)* map, const K* key)\
{\
    uint32_t hash = hashPtr(key, sizeof(K));\
    int32_t idx = hashmapIndex(K, V)(map, key);\
    \
    if(idx < 0) return;\
    \
    map->map[idx].hash |= INVALID_HASH;\
    --(map->size);\
}\
\
void hashmapClear(K, V)(Hashmap(K, V)* map)\
{\
    CAT_FREE(map->map);\
    hashmapInit(K, V)(map, STARTING_SIZE);\
}\
\
HashmapIterator(K, V) hashmapBegin(K, V)(const Hashmap(K, V)* map)\
{\
    HashmapIterator(K, V) itr = {map, 0};\
    while(!hashmapIteratorEnd(K, V)(&itr) && !map->map[itr.idx].hash && !(map->map[itr.idx].hash & INVALID_HASH))\
    {\
        ++itr.idx;\
    }\
    return itr;\
}\
\
bool hashmapIteratorNext(K, V)(HashmapIterator(K, V)* itr)\
{\
    do\
    {\
        ++itr->idx;\
    }\
    while(!hashmapIteratorEnd(K, V)(itr) && !itr->map->map[itr->idx].hash && !(itr->map->map[itr->idx].hash & INVALID_HASH));\
    return !hashmapIteratorEnd(K, V)(itr);\
}\

#define keyEQ(K, V) keyEQ ## K ## V

#define HASHMAP_IMPL(K, V)\
static inline bool keyEQ(K, V)(const K* lhs, const K* rhs){return *rhs == *lhs;}\
HASHMAP_IMPL_EQ(K, V, keyEQ(K, V))

#define HASHMAP_IMPL_CMP(K, V, CMP) HASHMAP_IMPL_EQ(K, V, !CMP)

#include "containers/hashmapImpls.h"
