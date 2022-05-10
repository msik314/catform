#include "util/globalDefs.h"
#include "components/bulletComponent.h"

#include "ecs/ectColumn.h"
#include "ecs/ectColumn.inl"
#include "json/jsonData.h"
#include "json/jsonTypes.h"

void serializeBulletComponents(const ECTColumn* colGen, JsonData* data, uint32_t column)
{
    const ECTColumn(BulletComponent)* bulletComponents = (ECTColumn(BulletComponent)*)colGen;
    uint32_t objIdx;
    uint32_t transform;
    
    for(uint32_t i = 0; i < bulletComponents->components.size; ++i)
    {
        objIdx = jsonDataArrayAddObject(data, column);
        
        jsonDataAddInt(data, objIdx, jsonKey("id"), (int32_t)bulletComponents->components.data[i].self.id);
        
        jsonDataAddInt(data, objIdx, jsonKey("flags"), (int32_t)bulletComponents->components.data[i].self.flags);
        
        if(bulletComponents->components.data[i].self.parent == INVALID_OBJECT)
        {
            jsonDataAddNull(data, objIdx, jsonKey("parent"));
        }
        else
        {
            jsonDataAddInt(data, objIdx, jsonKey("parent"), (int32_t)bulletComponents->components.data[i].self.parent);
        }
        
        
        jsonDataAddVec2(data, objIdx, jsonKey("velocity"), bulletComponents->components.data[i].velocity);
        jsonDataAddReal(data, objIdx, jsonKey("gravity"), (int32_t)bulletComponents->components.data[i].gravity);
        jsonDataAddReal(data, objIdx, jsonKey("fallDelay"), (int32_t)bulletComponents->components.data[i].fallDelay);
        jsonDataAddReal(data, objIdx, jsonKey("damage"), (int32_t)bulletComponents->components.data[i].damage);
        jsonDataAddReal(data, objIdx, jsonKey("knockBack"), (int32_t)bulletComponents->components.data[i].knockBack);
        jsonDataAddReal(data, objIdx, jsonKey("lifeTime"), (int32_t)bulletComponents->components.data[i].lifeTime);
    }
}

void deserializeBulletComponents(ECTColumn* colGen, const JsonData* data, uint32_t column, const Hashmap(ObjectID, ObjectID)* refMap)
{
    ECTColumn(BulletComponent)* bulletComponents = (ECTColumn(BulletComponent)*)colGen;
    const JsonObject* array;
    const JsonObject* component;
    double realVal;
    uint32_t index;
    int32_t intVal;
    ObjectID translatedId;
    
    BulletComponent b;
    
    array = jsonDataGetChildConst(data, column);
    
    for(uint32_t i = 0; i < jsonArraySize(array); ++i)
    {
        jsonArrayGetIndex(array, i, &index);
        component = jsonDataGetChildConst(data, index);
        
        jsonObjectGetKey(component, "id", &intVal);
        b.self.id = (ObjectID)intVal;
        
        jsonObjectGetKey(component, "flags", &intVal);
        b.self.flags = (uint32_t)intVal;
        
        if(jsonObjectGetKey(component, "parent", &intVal) != JSON_TYPE_NULL)
        {
            b.self.parent = (ObjectID)intVal;
        }
        else
        {
            b.self.parent = INVALID_OBJECT;
        }
        
        jsonObjectGetKey(component, "velocity", &index);
        b.velocity = jsonDataGetVec2(data, index);
        jsonObjectGetKey(component, "gravity", &realVal);
        b.gravity = (float)realVal;
        jsonObjectGetKey(component, "fallDelay", &realVal);
        b.fallDelay = (float)realVal;
        jsonObjectGetKey(component, "damage", &realVal);
        b.damage = (float)realVal;
        jsonObjectGetKey(component, "knockBack", &realVal);
        b.knockBack = (float)realVal;
        jsonObjectGetKey(component, "lifeTime", &realVal);
        b.lifeTime = (float)realVal;
        
        hashmapGet(ObjectID, ObjectID)(refMap, &b.self.id, &translatedId);
        b.self.id = translatedId;
        
        hashmapGet(ObjectID, ObjectID)(refMap, &b.self.parent, &translatedId);
        b.self.parent = translatedId;
        
        ectColumnAdd(BulletComponent)(bulletComponents, &b);
    }
    
}

ECTCOLUMN_IMPL_SER(BulletComponent, serializeBulletComponents, deserializeBulletComponents)
