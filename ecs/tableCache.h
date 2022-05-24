#ifndef TABLE_CACHE_H
#define TABLE_CACHE_H

#include <stdint.h>

#include "core/tag.h"
#include "containers/ring.h"
#include "containers/hashmap.h"
#include "ecs/ecTable.h"
#include "json/jsonData.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define CAT_MAX_LOADED_TABLES 8

typedef struct
{
    uint32_t index:31;
    uint32_t free:1;
    uint32_t opCounter;
}
TableSlot;

HASHMAP_DECL(Tag, TableSlot);

typedef struct
{
    Tag name;
    uint32_t opCounter;
}
GarbageSlot;

RING_DECL(GarbageSlot);

typedef struct
{
    ECTable loadedTables[CAT_MAX_LOADED_TABLES];
    uint32_t freeIdx;
    
    Hashmap(Tag, TableSlot) tableMap;
    
    Ring(GarbageSlot) garbage;
}
TableCache;

TableCache* tableCacheGetInstance();

void tableCacheCreate(TableCache* tableCache);
void tableCacheDestroy(TableCache* tableCache);

ECTable* tableCacheLoad(TableCache* tableCache, Tag name, const JsonData* data, uint32_t parentObject);

void tableCacheFree(TableCache* tableCache, Tag name);
void tableCacheRealloc(TableCache* tableCache, Tag name);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //TABLE_CACHE_H
