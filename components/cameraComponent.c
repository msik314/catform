#include "util/globalDefs.h"
#include "components/cameraComponent.h"

#include "ecs/ectColumn.h"
#include "ecs/ectColumn.inl"
#include "json/jsonData.h"
#include "json/jsonTypes.h"

void serializeCameraComponents(const ECTColumn* colGen, JsonData* data, uint32_t column)
{
    const ECTColumn(CameraComponent)* cameraComponents = (ECTColumn(CameraComponent)*)colGen;
    uint32_t objIdx;
    uint32_t transform;
    
    for(uint32_t i = 0; i < cameraComponents->components.size; ++i)
    {
        objIdx = jsonDataArrayAddObject(data, column);
        
        jsonDataAddInt(data, objIdx, jsonKey("id"), (int32_t)cameraComponents->components.data[i].self.id);
        
        jsonDataAddInt(data, objIdx, jsonKey("flags"), (int32_t)cameraComponents->components.data[i].self.flags);
        
        if(cameraComponents->components.data[i].self.parent == INVALID_OBJECT)
        {
            jsonDataAddNull(data, objIdx, jsonKey("parent"));
        }
        else
        {
            jsonDataAddInt(data, objIdx, jsonKey("parent"), (int32_t)cameraComponents->components.data[i].self.parent);
        }
        
        jsonDataAddMat4(data, objIdx, jsonKey("projection"), &cameraComponents->components.data[i].projection);
    }
}

void deserializeCameraComponents(ECTColumn* colGen, const JsonData* data, uint32_t column, const Hashmap(ObjectID, ObjectID)* refMap)
{
    ECTColumn(CameraComponent)* cameraComponents = (ECTColumn(CameraComponent)*)colGen;
    const JsonObject* array;
    const JsonObject* component;
    int32_t index;
    int32_t intVal;
    ObjectID translatedId;
    
    CameraComponent cc;
    
    array = jsonDataGetChildConst(data, column);
    
    for(uint32_t i = 0; i < jsonArraySize(array); ++i)
    {
        jsonArrayGetIndex(array, i, &index);
        component = jsonDataGetChildConst(data, index);
        
        jsonObjectGetKey(component, "id", &intVal);
        cc.self.id = (ObjectID)intVal;
        
        jsonObjectGetKey(component, "flags", &intVal);
        cc.self.flags = (uint32_t)intVal;
        
        if(jsonObjectGetKey(component, "parent", &intVal) != JSON_TYPE_NULL)
        {
            cc.self.parent = (ObjectID)intVal;
        }
        else
        {
            cc.self.parent = INVALID_OBJECT;
        }
        
        jsonObjectGetKey(component, "projection", &index);
        jsonDataGetMat4(data, index, &cc.projection);
        
        hashmapGet(ObjectID, ObjectID)(refMap, &cc.self.id, &translatedId);
        cc.self.id = translatedId;
        
        hashmapGet(ObjectID, ObjectID)(refMap, &cc.self.parent, &translatedId);
        cc.self.parent = translatedId;
        
        ectColumnAdd(CameraComponent)(cameraComponents, &cc);
    }
    
}

ECTCOLUMN_IMPL_SER(CameraComponent, serializeCameraComponents, deserializeCameraComponents)
