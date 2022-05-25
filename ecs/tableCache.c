#include "util/globaldefs.h"
#include "ecs/tableCache.h"

#include <string.h>
#include "core/tag.h"
#include "containers/ring.h"
#include "containers/hashmap.h"
#include "ecs/ecTable.h"
#include "components/component.h"
#include "json/jsonData.h"

TableCache* tableCacheGetInstance()
{
    static TableCache cache;
    return &cache;
}

void tableCacheCreate(TableCache* tableCache)
{
    memset(tableCache->loadedTables, 0, sizeof(tableCache->loadedTables));
    tableCache->freeIdx = 0;
    hashmapCreate(Tag, TableSlot)(&tableCache->tableMap);
    ringCreate(GarbageSlot)(&tableCache->garbage);
}

void tableCacheDestroy(TableCache* tableCache)
{
    for(uint32_t i = 0; i < CAT_MAX_LOADED_TABLES && i < tableCache->freeIdx; ++i)
    {
        ecTableDestroy(&tableCache->loadedTables[i]);
    }
    
    hashmapDestroy(Tag, TableSlot)(&tableCache->tableMap);
    ringDestroy(GarbageSlot)(&tableCache->garbage);
}

ECTable* tableCacheGet(TableCache* tableCache, Tag name)
{
    TableSlot slot = {};
    
    if(hashmapGet(Tag, TableSlot)(&tableCache->tableMap, &name, &slot))
    {
        tableCacheRealloc(tableCache, name);
        return &tableCache->loadedTables[slot.index];
    }
    
    return NULL;
}

ECTable* tableCacheLoad(TableCache* tableCache, Tag name, const JsonData* data, uint32_t parentObject)
{
    TableSlot slot = {};
    GarbageSlot garbageSlot = {};
    
    if(hashmapGet(Tag, TableSlot)(&tableCache->tableMap, &name, &slot))
    {
        tableCacheRealloc(tableCache, name);
        return &tableCache->loadedTables[slot.index];
    }
    
    if(tableCache->freeIdx < CAT_MAX_LOADED_TABLES)
    {
        slot.index = tableCache->freeIdx++;
        ecTableCreate(&tableCache->loadedTables[slot.index], NUM_COMPONENT_TYPES);
        ecTableDeserialize(&tableCache->loadedTables[slot.index], data, parentObject);
        ecTableAddRemove(&tableCache->loadedTables[slot.index]);
        hashmapSet(Tag, TableSlot)(&tableCache->tableMap, &name, &slot);
        return &tableCache->loadedTables[slot.index];
    }
    
    while(ringGet(GarbageSlot)(&tableCache->garbage, &garbageSlot))
    {
        ringPop(GarbageSlot)(&tableCache->garbage);
        if(!hashmapGet(Tag, TableSlot)(&tableCache->tableMap, &garbageSlot.name, &slot)) continue;
        if(slot.opCounter != garbageSlot.opCounter) continue;
        if(!slot.free) continue;
        
        hashmapRemove(Tag, TableSlot)(&tableCache->tableMap, &garbageSlot.name);
        slot.opCounter++;
        ecTableDestroy(&tableCache->loadedTables[slot.index]);
        
        ecTableCreate(&tableCache->loadedTables[slot.index], NUM_COMPONENT_TYPES);
        ecTableDeserialize(&tableCache->loadedTables[slot.index], data, parentObject);
        ecTableAddRemove(&tableCache->loadedTables[slot.index]);
        hashmapSet(Tag, TableSlot)(&tableCache->tableMap, &name, &slot);
        return &tableCache->loadedTables[slot.index];
    }
    
    return NULL;
}

void tableCacheFree(TableCache* tableCache, Tag name)
{
    TableSlot slot = {};
    GarbageSlot garbageSlot = {};
    
    if(!hashmapGet(Tag, TableSlot)(&tableCache->tableMap, &name, &slot)) return;
    
    garbageSlot.name = name;
    garbageSlot.opCounter = slot.opCounter;
    
    slot.free = 1;
    hashmapSet(Tag, TableSlot)(&tableCache->tableMap, &name, &slot);
    ringAdd(GarbageSlot)(&tableCache->garbage, &garbageSlot);
}

void tableCacheRealloc(TableCache* tableCache, Tag name)
{
    TableSlot slot = {};
    
    if(!hashmapGet(Tag, TableSlot)(&tableCache->tableMap, &name, &slot)) return;
    
    slot.free = 0;
    hashmapSet(Tag, TableSlot)(&tableCache->tableMap, &name, &slot);
}
