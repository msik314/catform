#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    void* map;
    uint32_t size;
    uint32_t capacity;
}
Hashmap;

#define Hashmap(K, V) Hashmap ## K ## V
#define HashmapLine(K, V) HashmapLine ## K ## V
#define HashmapIterator(K, V) HashmapIterator ## K ## V
    
#define hashmapCreate(K, V) hashmap ## K ## V ## Create
#define hashmapDestroy(K, V) hashmap ## K ## V ## Destroy
#define hashmapGet(K, V) hashmap ## K ## V ## Get
#define hashmapSet(K, V) hashmap ## K ## V ## Set
#define hashmapRemove(K, V) hashmap ## K ## V ## Remove
#define hashmapClear(K, V) hashmap ## K ## V ## Clear
#define hashmapBegin(K, V) hashmap ## K ## V ## Begin
#define hashmapIteratorEnd(K, V) hashmapIterator ## K ## V ## End
#define hashmapIteratorKey(K, V) hashmapIterator ## K ## V ## Key
#define hashmapIteratorValue(K, V) hashmapIterator ## K ## V ## Value
#define hashmapIteratorNext(K, V) hashmapIterator ## K ## V ## Next

#define HASHMAP_DECL(K, V)\
typedef struct\
{\
    K key;\
    uint32_t hash;\
    V value;\
}\
HashmapLine(K, V);\
\
typedef struct\
{\
    HashmapLine(K, V)* map;\
    uint32_t size;\
    uint32_t capacity;\
}\
Hashmap(K, V);\
\
void hashmapCreate(K, V)(Hashmap(K, V)* map);\
void hashmapDestroy(K, V)(Hashmap(K, V)* map);\
int32_t hashmapGet(K, V)(Hashmap(K, V)* map, const K* key, V* outValue);\
void hashmapSet(K, V)(Hashmap(K, V)* map, const K* key, const V* value);\
void hashmapRemove(K, V)(Hashmap(K, V)* map, const K* key);\
void hashmapClear(K, V)(Hashmap(K, V)* map);\
\
typedef struct\
{\
    Hashmap(K, V)* map;\
    uint32_t idx;\
}\
HashmapIterator(K, V);\
\
HashmapIterator(K, V) hashmapBegin(K, V)(Hashmap(K, V)* map);\
bool hashmapIteratorNext(K, V)(HashmapIterator(K, V)* itr);\
\
static inline bool hashmapIteratorEnd(K, V)(const HashmapIterator(K, V)* itr)\
{\
    return itr->idx >= itr->map->capacity;\
}\
\
static inline void hashmapIteratorKey(K, V)(const HashmapIterator(K, V)* itr, K* outKey)\
{\
    *outKey = itr->map->map[itr->idx].key;\
}\
\
static inline void hashmapIteratorValue(K, V)(const HashmapIterator(K, V)* itr, V* outValue)\
{\
    *outValue = itr->map->map[itr->idx].value;\
}\

#ifdef __cplusplus
};
#endif //__cplusplus


#endif //HASHMAP_H
