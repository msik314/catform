#include "util/globalDefs.h"
#include "components/playerComponent.h"

#include "ecs/ectColumn.h"
#include "ecs/ectColumn.inl"
#include "json/jsonData.h"
#include "json/jsonTypes.h"

void serializePlayerComponents(const ECTColumn* colGen, JsonData* data, uint32_t column)
{
    const ECTColumn(PlayerComponent)* playerComponents = (ECTColumn(PlayerComponent)*)colGen;
    uint32_t objIdx;
    uint32_t transform;
    
    for(uint32_t i = 0; i < playerComponents->components.size; ++i)
    {
        objIdx = jsonDataArrayAddObject(data, column);
        
        jsonDataAddInt(data, objIdx, jsonKey("id"), (int32_t)playerComponents->components.data[i].self.id);
        
        jsonDataAddInt(data, objIdx, jsonKey("flags"), (int32_t)playerComponents->components.data[i].self.flags);
        
        if(playerComponents->components.data[i].self.parent == INVALID_OBJECT)
        {
            jsonDataAddNull(data, objIdx, jsonKey("parent"));
        }
        else
        {
            jsonDataAddInt(data, objIdx, jsonKey("parent"), (int32_t)playerComponents->components.data[i].self.parent);
        }
        
        jsonDataAddReal(data, objIdx, jsonKey("moveSpeed"), playerComponents->components.data[i].moveSpeed);
        jsonDataAddReal(data, objIdx, jsonKey("jumpSpeed"), playerComponents->components.data[i].jumpSpeed);
        jsonDataAddReal(data, objIdx, jsonKey("groundAccel"), playerComponents->components.data[i].groundAccel);
        jsonDataAddReal(data, objIdx, jsonKey("airAccel"), playerComponents->components.data[i].airAccel);
        jsonDataAddInt(data, objIdx, jsonKey("playerNum"), (int32_t)playerComponents->components.data[i].playerNum);
        jsonDataAddInt(data, objIdx, jsonKey("controller1"), (int32_t)playerComponents->components.data[i].controller1);
        jsonDataAddInt(data, objIdx, jsonKey("controller2"), (int32_t)playerComponents->components.data[i].controller2);
        jsonDataAddInt(data, objIdx, jsonKey("vertical"), (int32_t)playerComponents->components.data[i].vertical);
        jsonDataAddInt(data, objIdx, jsonKey("horizontal"), (int32_t)playerComponents->components.data[i].horizontal);
        jsonDataAddInt(data, objIdx, jsonKey("jumpBtn"), (int32_t)playerComponents->components.data[i].jumpBtn);
        jsonDataAddInt(data, objIdx, jsonKey("shootBtn"), (int32_t)playerComponents->components.data[i].shootBtn);
        jsonDataAddVec2(data, objIdx, jsonKey("velocity"), playerComponents->components.data[i].velocity);
        jsonDataAddInt(data, objIdx, jsonKey("colliding"), (int32_t)playerComponents->components.data[i].colliding);
        jsonDataAddReal(data, objIdx, jsonKey("lastDirection"), playerComponents->components.data[i].lastDirection);
    }
}

void deserializePlayerComponents(ECTColumn* colGen, const JsonData* data, uint32_t column, const Hashmap(ObjectID, ObjectID)* refMap)
{
    ECTColumn(PlayerComponent)* playerComponents = (ECTColumn(PlayerComponent)*)colGen;
    const JsonObject* array;
    const JsonObject* component;
    double realVal;
    uint32_t index;
    int32_t intVal;
    ObjectID translatedId;
    
    PlayerComponent pc;
    
    array = jsonDataGetChildConst(data, column);
    
    for(uint32_t i = 0; i < jsonArraySize(array); ++i)
    {
        jsonArrayGetIndex(array, i, &index);
        component = jsonDataGetChildConst(data, index);
        
        jsonObjectGetKey(component, "id", &intVal);
        pc.self.id = (ObjectID)intVal;
        
        jsonObjectGetKey(component, "flags", &intVal);
        pc.self.flags = (uint32_t)intVal;
        
        if(jsonObjectGetKey(component, "parent", &intVal) != JSON_TYPE_NULL)
        {
            pc.self.parent = (ObjectID)intVal;
        }
        else
        {
            pc.self.parent = INVALID_OBJECT;
        }
        
        jsonObjectGetKey(component, "moveSpeed", &realVal);
        pc.moveSpeed = (float)realVal;
        jsonObjectGetKey(component, "jumpSpeed", &realVal);
        pc.jumpSpeed = (float)realVal;
        jsonObjectGetKey(component, "groundAccel", &realVal);
        pc.groundAccel = (float)realVal;
        jsonObjectGetKey(component, "airAccel", &realVal);
        pc.airAccel = (float)realVal;
        jsonObjectGetKey(component, "playerNum", &intVal);
        pc.playerNum = (uint32_t)intVal;
        jsonObjectGetKey(component, "controller1", &intVal);
        pc.controller1 = (uint32_t)intVal;
        jsonObjectGetKey(component, "controller2", &intVal);
        pc.controller2 = (uint32_t)intVal;
        jsonObjectGetKey(component, "vertical", &intVal);
        pc.vertical = (uint32_t)intVal;
        jsonObjectGetKey(component, "horizontal", &intVal);
        pc.horizontal = (uint32_t)intVal;
        jsonObjectGetKey(component, "jumpBtn", &intVal);
        pc.jumpBtn = (uint32_t)intVal;
        jsonObjectGetKey(component, "shootBtn", &intVal);
        pc.shootBtn = (uint32_t)intVal;
        jsonObjectGetKey(component, "velocity", &index);
        pc.velocity = jsonDataGetVec2(data, index);
        jsonObjectGetKey(component, "colliding", &intVal);
        pc.colliding = intVal;
        jsonObjectGetKey(component, "lastDirection", &realVal);
        pc.lastDirection = (float)realVal;
        
        hashmapGet(ObjectID, ObjectID)(refMap, &pc.self.id, &translatedId);
        pc.self.id = translatedId;
        
        hashmapGet(ObjectID, ObjectID)(refMap, &pc.self.parent, &translatedId);
        pc.self.parent = translatedId;
        
        ectColumnAdd(PlayerComponent)(playerComponents, &pc);
    }
    
}

ECTCOLUMN_IMPL_SER(PlayerComponent, serializePlayerComponents, deserializePlayerComponents)
