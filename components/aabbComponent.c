#include "util/globalDefs.h"
#include "components/aabbComponent.h"

#include "ecs/ectColumn.h"
#include "ecs/ectColumn.inl"
#include "cmath/cvec.h"
#include "json/jsonData.h"
#include "json/jsonTypes.h"

void serializeAabbComponents(const ECTColumn* colGen, JsonData* data, uint32_t column)
{
    const ECTColumn(AabbComponent)* aabbComponents = (ECTColumn(AabbComponent)*)colGen;
    uint32_t objIdx;
    uint32_t transform;
    
    for(uint32_t i = 0; i < aabbComponents->components.size; ++i)
    {
        objIdx = jsonDataArrayAddObject(data, column);
        
        jsonDataAddInt(data, objIdx, jsonKey("id"), (int32_t)aabbComponents->components.data[i].self.id);
        
        jsonDataAddInt(data, objIdx, jsonKey("flags"), (int32_t)aabbComponents->components.data[i].self.flags);
        
        if(aabbComponents->components.data[i].self.parent == INVALID_OBJECT)
        {
            jsonDataAddNull(data, objIdx, jsonKey("parent"));
        }
        else
        {
            jsonDataAddInt(data, objIdx, jsonKey("parent"), (int32_t)aabbComponents->components.data[i].self.parent);
        }
        
        jsonDataAddVec2(data, objIdx, jsonKey("offset"), aabbComponents->components.data[i].offset);
        jsonDataAddVec2(data, objIdx, jsonKey("size"), aabbComponents->components.data[i].size);
    }
}

void deserializeAabbComponents(ECTColumn* colGen, const JsonData* data, uint32_t column, const Hashmap(ObjectID, ObjectID)* refMap)
{
    ECTColumn(AabbComponent)* aabbComponents = (ECTColumn(AabbComponent)*)colGen;
    const JsonObject* array;
    const JsonObject* component;
    int32_t index;
    int32_t intVal;
    uint32_t objIdx;
    ObjectID translatedId;
    
    AabbComponent aabb;
    
    array = jsonDataGetChildConst(data, column);
    
    for(uint32_t i = 0; i < jsonArraySize(array); ++i)
    {
        jsonArrayGetIndex(array, i, &index);
        component = jsonDataGetChildConst(data, index);
        
        jsonObjectGetKey(component, "id", &intVal);
        aabb.self.id = (ObjectID)intVal;
        
        jsonObjectGetKey(component, "flags", &intVal);
        aabb.self.flags = (uint32_t)intVal;
        
        if(jsonObjectGetKey(component, "parent", &intVal) != JSON_TYPE_NULL)
        {
            aabb.self.parent = (ObjectID)intVal;
        }
        else
        {
            aabb.self.parent = INVALID_OBJECT;
        }
        
        jsonObjectGetKey(component, "offset", &objIdx);
        aabb.offset = jsonDataGetVec2(data, objIdx);
        jsonObjectGetKey(component, "size", &objIdx);
        aabb.size = jsonDataGetVec2(data, objIdx);
        
        hashmapGet(ObjectID, ObjectID)(refMap, &aabb.self.id, &translatedId);
        aabb.self.id = translatedId;
        
        hashmapGet(ObjectID, ObjectID)(refMap, &aabb.self.parent, &translatedId);
        aabb.self.parent = translatedId;
        
        ectColumnAdd(AabbComponent)(aabbComponents, &aabb);
    }
    
}

ECTCOLUMN_IMPL_SER(AabbComponent, serializeAabbComponents, deserializeAabbComponents)
