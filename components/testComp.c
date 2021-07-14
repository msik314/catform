#include "util/globalDefs.h"
#include "components/testComp.h"

#include "ecs/ectColumn.h"
#include "ecs/ectColumn.inl"

void serializeTestComps(const ECTColumn* colGen, JsonData* data, uint32_t column)
{
    const ECTColumn(TestComp)* testComps = (ECTColumn(TestComp)*)colGen;
    JsonValue val;
    uint32_t objIdx;
    uint32_t transform;
    
    for(uint32_t i = 0; i < testComps->components.size; ++i)
    {
        objIdx = jsonDataArrayAddObject(data, column);
        
        val.type = JSON_TYPE_INT;
        val.intValue = (int32_t)testComps->components.data[i].self.id;
        jsonDataAddValue(data, objIdx, jsonKey("id"), val);
        
        val.intValue = (int32_t)testComps->components.data[i].self.flags;
        jsonDataAddValue(data, objIdx, jsonKey("flags"), val);
        
        val.intValue = (int32_t)testComps->components.data[i].self.parent;
        jsonDataAddValue(data, objIdx, jsonKey("parent"), val);
        
        val.intValue = (int32_t)testComps->components.data[i].number;
        jsonDataAddValue(data, objIdx, jsonKey("number"), val);
    }
}

void deserializeTestComps(ECTColumn* colGen, const JsonData* data, uint32_t column, const Hashmap(ObjectID, ObjectID)* refMap)
{
    ECTColumn(TestComp)* testComps = (ECTColumn(TestComp)*)colGen;
    const JsonObject* array;
    const JsonObject* component;
    const JsonObject* transform;
    int32_t index;
    int32_t intVal;
    const char* stringVal;
    ObjectID translatedId;
    
    TestComp c;
    
    array = jsonDataGetChildConst(data, column);
    
    for(uint32_t i = 0; i < jsonArraySize(array); ++i)
    {
        jsonArrayGetIndex(array, i, &index);
        component = jsonDataGetChildConst(data, index);
        
        jsonObjectGetKey(component, "id", &intVal);
        c.self.id = (ObjectID)intVal;
        
        jsonObjectGetKey(component, "flags", &intVal);
        c.self.flags = (uint32_t)intVal;
        
        jsonObjectGetKey(component, "parent", &intVal);
        c.self.parent = (ObjectID)intVal;
        
        jsonObjectGetKey(component, "number", &intVal);
        c.number = (uint32_t)intVal;
        
        hashmapGet(ObjectID, ObjectID)(refMap, &c.self.id, &translatedId);
        c.self.id = translatedId;
        
        hashmapGet(ObjectID, ObjectID)(refMap, &c.self.parent, &translatedId);
        c.self.parent = translatedId;
        
        ectColumnAdd(TestComp)(testComps, &c);
    }
    
}

ECTCOLUMN_IMPL_SER(TestComp, serializeTestComps, deserializeTestComps);
