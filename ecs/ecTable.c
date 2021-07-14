#include "util/globalDefs.h"
#include "ecs/ecTable.h"

#include <string.h>
#include "ecs/object.h"
#include "ecs/ectVirtualTable.h"
#include "ecs/ectColumn.h"
#include "ecs/pointerMap.h"
#include "components/component.h"
#include "components/entity.h"
#include "core/error.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

#define MAX_OBJECTS 4096

void ecTableCreate(ECTable* table, uint32_t numColumns)
{
    table->columns = (ECTColumn*)CAT_MALLOC(numColumns * sizeof(ECTColumn));
    table->numColumns = numColumns;
    
    memset(table->columns, 0, numColumns * sizeof(ECTColumn));
    
    pointerMapCreate(&table->pointerMap, MAX_OBJECTS);
}

void ecTableDestroy(ECTable* table)
{
    pointerMapDestroy(&table->pointerMap);
    
    for(uint32_t i = 0; i < table->numColumns; ++i)
    {
        ectColumnDestroyGeneric(&table->columns[i]);
    }
}

void ecTableMark(ECTable* table)
{
    for(uint32_t i = 0; i < table->numColumns; ++i)
    {
        getVirtualParentDelete(i)(&table->columns[i], &table->columns[COMPONENT(Entity)], &table->pointerMap);
    }
}

void ecTableAddRemove(ECTable* table)
{
    for(uint32_t i = 0; i < table->numColumns; ++i)
    {
        getVirtualAddRemove(i)(&table->columns[i], &table->pointerMap);
    }
    
    for(uint32_t i = 0; i < table->numColumns; ++i)
    {
        getVirtualParentAdd(i)(&table->columns[i], &table->columns[COMPONENT(Entity)], &table->pointerMap);
    }
}

void ecTableRemoveAll(ECTable* table)
{
    for(uint32_t i = 0; i < table->numColumns; ++i)
    {
        getVirtualRemoveAll(i)(&table->columns[i], &table->pointerMap);
    }
}

uint32_t ecTableGetChildren(const ECTable* table, ObjectID entity, ObjectID* outEntities, uint32_t maxOut)
{
    uint32_t numFound = 0;
    for(uint32_t i = 0; i < table->columns[COMPONENT(Entity)].components.size; ++i)
    {
        if(((ECTColumn(Entity)*)&table->columns[COMPONENT(Entity)])->components.data[i].self.parent == entity)
        {
            outEntities[numFound] = ((ECTColumn(Entity)*)&table->columns[COMPONENT(Entity)])->components.data[i].self.id;
            ++numFound;
            if(numFound == maxOut) return numFound;
        }
    }
    
    return numFound;
}

uint32_t ecTableGetMarkFuns(const ECTable* table, ECTColumnParentFun* outFuns, uint32_t maxFuns)
{
    uint32_t idx;
    for(idx = 0; idx < maxFuns && idx < table->numColumns; ++idx)
    {
        outFuns[idx] = getVirtualParentDelete(idx);
    }
    
    return idx;
}

uint32_t ecTableGetARFuns(const ECTable* table, ECTColumnAddRemoveFun* outFuns, uint32_t maxFuns)
{
    uint32_t idx;
    for(idx = 0; idx < maxFuns && idx < table->numColumns; ++idx)
    {
        outFuns[idx] = getVirtualAddRemove(idx);
    }
    
    return idx;
}

uint32_t ecTableGetParentFuns(const ECTable* table, ECTColumnParentFun* outFuns, uint32_t maxFuns)
{
    uint32_t idx;
    for(idx = 0; idx < maxFuns && idx < table->numColumns; ++idx)
    {
        outFuns[idx] = getVirtualParentAdd(idx);
    }
    
    return idx;
}

ObjectID ecTableMap(ECTable* table, Object* object)
{
    object->id = pointerMapAdd(&table->pointerMap, INVALID_COLUMN_INDEX);
    return object->id;
}

void ecTableSerialize(const ECTable* table, JsonData* data, uint32_t parentObject)
{
    ECTColumnSerializeFun colFun;
    uint32_t parentArray = jsonDataAddArray(data, parentObject, jsonKey("table"));
    
    int32_t childArray;
    
    for(uint32_t i = 0; i < table->numColumns; ++i)
    {
        childArray = jsonDataArrayAddArray(data, parentArray);
        
        colFun = getVirtualSerialize(i);
        if(!colFun) continue;
        
        colFun(&table->columns[i], data, childArray);
    }
}

void ecTableDeserialize(ECTable* table, const JsonData* data, uint32_t parentObject)
{
    Hashmap(ObjectID, ObjectID) idMap = {};
    ECTColumnDeserializeFun colFun;
    const JsonObject* parent;
    const JsonObject* colObject;
    const JsonObject* component;
    int32_t colIdx;
    int32_t compIdx;
    JsonType type;
    ObjectID objId = INVALID_OBJECT;
    ObjectID newId = INVALID_OBJECT;
    
    hashmapCreate(ObjectID, ObjectID)(&idMap);
    hashmapSet(ObjectID, ObjectID)(&idMap, &objId, &newId);
    
    parent = jsonDataGetChildConst(data, parentObject);
    
    for(uint32_t i = 0; i < table->numColumns; ++i)
    {
        jsonArrayGetIndex(parent, i, &colIdx);
        colObject = jsonDataGetChildConst(data, colIdx);
        
        for(uint32_t j = 0; j <  jsonArraySize(colObject); ++j)
        {
            jsonArrayGetIndex(colObject, j, &compIdx);
            component = jsonDataGetChildConst(data, compIdx);
            
            type = jsonObjectGetKey(component, "id", &objId);
            if(type == JSON_TYPE_UNKNOWN) continue;
            
            newId = pointerMapAdd(&table->pointerMap, INVALID_COLUMN_INDEX);
            hashmapSet(ObjectID, ObjectID)(&idMap, &objId, &newId);
        }
    }
    
    for(uint32_t i = 0; i < table->numColumns; ++i)
    {
        colFun = getVirtualDeserialize(i);
        if(!colFun) continue;
        
        jsonArrayGetIndex(parent, i, &colIdx);
        colFun(&table->columns[i], data, colIdx, &idMap);
    }
    
    hashmapDestroy(ObjectID, ObjectID)(&idMap);
}
