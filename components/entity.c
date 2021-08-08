#include "util/globalDefs.h"
#include "components/entity.h"

#include <string.h>
#include "ecs/ectColumn.h"
#include "ecs/ectColumn.inl"
#include "ecs/pointerMap.h"
#include "json/jsonData.h"
#include "json/jsonTypes.h"
#include "core/tag.h"

void serializeEntities(const ECTColumn* colGen, JsonData* data, uint32_t column)
{
    const ECTColumn(Entity)* entities = (ECTColumn(Entity)*)colGen;
    JsonValue val;
    uint32_t objIdx;
    uint32_t transform;
    
    for(uint32_t i = 0; i < entities->components.size; ++i)
    {
        objIdx = jsonDataArrayAddObject(data, column);
        
        val.type = JSON_TYPE_INT;
        val.intValue = (int32_t)entities->components.data[i].self.id;
        jsonDataAddValue(data, objIdx, jsonKey("id"), val);
        
        val.intValue = (int32_t)entities->components.data[i].self.flags;
        jsonDataAddValue(data, objIdx, jsonKey("flags"), val);
        
        if(entities->components.data[i].self.parent == INVALID_OBJECT)
        {
            val.type = JSON_TYPE_NULL;
            val.intValue = INVALID_OBJECT;
        }
        else
        {
            val.intValue = (int32_t)entities->components.data[i].self.parent;
        }
        
        jsonDataAddValue(data, objIdx, jsonKey("parent"), val);
        
        jsonDataAddTag(data, objIdx, jsonKey("name"), entities->components.data[i].name);
        
        transform = jsonDataAddObject(data, objIdx, jsonKey("transform"));
        jsonDataAddQuat(data, transform, jsonKey("rotation"), entities->components.data[i].transform.rotation);
        jsonDataAddVec3(data, transform, jsonKey("position"), entities->components.data[i].transform.position);
        jsonDataAddVec3(data, transform, jsonKey("scale"), entities->components.data[i].transform.scale);
    }
}

void deserializeEntities(ECTColumn* colGen, const JsonData* data, uint32_t column, const Hashmap(ObjectID, ObjectID)* refMap)
{
    ECTColumn(Entity)* entities = (ECTColumn(Entity)*)colGen;
    const JsonObject* array;
    const JsonObject* component;
    const JsonObject* transform;
    int32_t index;
    int32_t intVal;
    const char* stringVal;
    ObjectID translatedId;
    
    Entity e;
    
    array = jsonDataGetChildConst(data, column);
    
    for(uint32_t i = 0; i < jsonArraySize(array); ++i)
    {
        jsonArrayGetIndex(array, i, &index);
        component = jsonDataGetChildConst(data, index);
        
        jsonObjectGetKey(component, "id", &intVal);
        e.self.id = (ObjectID)intVal;
        
        jsonObjectGetKey(component, "flags", &intVal);
        e.self.flags = (uint32_t)intVal;
        
        if(jsonObjectGetKey(component, "parent", &intVal) != JSON_TYPE_NULL)
        {
            e.self.parent = (ObjectID)intVal;
        }
        else
        {
            e.self.parent = INVALID_OBJECT;
        }
        
        jsonObjectGetKey(component, "name", &stringVal);
        tagSet(&e.name, stringVal);
        
        jsonObjectGetKey(component, "transform", &index);
        transform = jsonDataGetChildConst(data, index);
        
        jsonObjectGetKey(transform, "rotation", &index);
        e.transform.rotation = jsonDataGetQuat(data, index);
        
        jsonObjectGetKey(transform, "position", &index);
        e.transform.position = jsonDataGetVec3(data, index);
        
        jsonObjectGetKey(transform, "scale", &index);
        e.transform.scale = jsonDataGetVec3(data, index);
        
        hashmapGet(ObjectID, ObjectID)(refMap, &e.self.id, &translatedId);
        e.self.id = translatedId;
        
        hashmapGet(ObjectID, ObjectID)(refMap, &e.self.parent, &translatedId);
        e.self.parent = translatedId;
        
        ectColumnAdd(Entity)(entities, &e);
    }
    
}

static inline uint32_t markIfParentMarked(Entity* entities, bool* visited, uint32_t idx, const PointerMap* map)
{
    uint32_t parentIdx;
    uint32_t flags;
    
    if(visited[idx]) return atomicLoad32(&entities[idx].self.flags) & OBJECT_FLAG_REMOVE;
    visited[idx] = true;
    
    if(entities[idx].self.parent == INVALID_OBJECT) return atomicLoad32(&entities[idx].self.flags) & OBJECT_FLAG_REMOVE;
    
    parentIdx = pointerMapGet(map, entities[idx].self.parent);
    flags = markIfParentMarked(entities, visited, parentIdx, map);
    return (fetchOr32(&entities[idx].self.flags, flags) | flags) & OBJECT_FLAG_REMOVE; 
}

void entityMark(ECTColumn* column, ECTColumn* _, PointerMap* map)
{
    ECTColumn(Entity)* entities = (ECTColumn(Entity)*)column;
    bool entityVisited[entities->components.size];
    Entity* entity;
    
    memset(entityVisited, 0, entities->components.size * sizeof(bool));
    
    for(uint32_t i = 0; i < entities->components.size; ++i)
    {
        if(markIfParentMarked(entities->components.data, entityVisited, i, map))
        {
            entityResetHasComponent(&entities->components.data[i], COMPONENT(Entity));
        }
    }
}

ECTCOLUMN_IMPL_SER(Entity, serializeEntities, deserializeEntities)
