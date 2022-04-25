#include "util/globalDefs.h"
#include "components/spriteComponent.h"

#include "ecs/ectColumn.h"
#include "ecs/ectColumn.inl"
#include "json/jsonData.h"
#include "json/jsonTypes.h"
#include "render/texture.h"

void serializeSpriteComponents(const ECTColumn* colGen, JsonData* data, uint32_t column)
{
    const ECTColumn(SpriteComponent)* spriteComponents = (ECTColumn(SpriteComponent)*)colGen;
    uint32_t objIdx;
    uint32_t transform;
    
    for(uint32_t i = 0; i < spriteComponents->components.size; ++i)
    {
        objIdx = jsonDataArrayAddObject(data, column);
        
        jsonDataAddInt(data, objIdx, jsonKey("id"), (int32_t)spriteComponents->components.data[i].self.id);
        
        jsonDataAddInt(data, objIdx, jsonKey("flags"), (int32_t)spriteComponents->components.data[i].self.flags);
        
        if(spriteComponents->components.data[i].self.parent == INVALID_OBJECT)
        {
            jsonDataAddNull(data, objIdx, jsonKey("parent"));
        }
        else
        {
            jsonDataAddInt(data, objIdx, jsonKey("parent"), (int32_t)spriteComponents->components.data[i].self.parent);
        }
        
        jsonDataAddTag(data, objIdx, jsonKey("texName"), spriteComponents->components.data[i].texName);
        jsonDataAddVec4(data, objIdx, jsonKey("tint"), spriteComponents->components.data[i].tint);
        
    }
}

void deserializeSpriteComponents(ECTColumn* colGen, const JsonData* data, uint32_t column, const Hashmap(ObjectID, ObjectID)* refMap)
{
    ECTColumn(SpriteComponent)* spriteComponents = (ECTColumn(SpriteComponent)*)colGen;
    const JsonObject* array;
    const JsonObject* component;
    int32_t index;
    int32_t intVal;
    ObjectID translatedId;
    const char* stringVal;
    
    SpriteComponent sc;
    
    array = jsonDataGetChildConst(data, column);
    
    for(uint32_t i = 0; i < jsonArraySize(array); ++i)
    {
        jsonArrayGetIndex(array, i, &index);
        component = jsonDataGetChildConst(data, index);
        
        jsonObjectGetKey(component, "id", &intVal);
        sc.self.id = (ObjectID)intVal;
        
        jsonObjectGetKey(component, "flags", &intVal);
        sc.self.flags = (uint32_t)intVal;
        
        if(jsonObjectGetKey(component, "parent", &intVal) != JSON_TYPE_NULL)
        {
            sc.self.parent = (ObjectID)intVal;
        }
        else
        {
            sc.self.parent = INVALID_OBJECT;
        }
        
        jsonObjectGetKey(component, "texName", &stringVal);
        tagSet(&sc.texName, stringVal);
        
        jsonObjectGetKey(component, "tint", &index);
        sc.tint = jsonDataGetVec4(data, index);
        
        hashmapGet(ObjectID, ObjectID)(refMap, &sc.self.id, &translatedId);
        sc.self.id = translatedId;
        
        hashmapGet(ObjectID, ObjectID)(refMap, &sc.self.parent, &translatedId);
        sc.self.parent = translatedId;
        
        ectColumnAdd(SpriteComponent)(spriteComponents, &sc);
    }
    
}

ECTCOLUMN_IMPL_SER(SpriteComponent, serializeSpriteComponents, deserializeSpriteComponents)
