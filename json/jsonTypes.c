#include "util/globalDefs.h"
#include "json/jsonTypes.h"

#include "json/jsonData.h"
#include "cmath/cvec.h"

static inline float getJsonReal(const JsonObject* array, uint32_t index)
{
    JsonValue val;
    
    switch(jsonArrayGetIndex(array, index, &val))
    {
    case JSON_TYPE_INT:
        return (float)val.intValue;
    
    case JSON_TYPE_REAL:
        return (float)val.realValue;
        
    default:
        return 0.0f;
    }
}

//Vec2
void jsonDataAddVec2(JsonData* data, uint32_t parent, Tag name, Vec2 value)
{
    JsonValue val;
    uint32_t array = jsonDataAddArray(data, parent, name);
    
    val.type = JSON_TYPE_REAL;
    val.realValue = (double)value.x;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.y;
    jsonDataArrayAddValue(data, array, val);
}

void jsonDataArrayAddVec2(JsonData* data, uint32_t parent, Vec2 value)
{
    JsonValue val;
    uint32_t array = jsonDataArrayAddArray(data, parent);
    
    val.type = JSON_TYPE_REAL;
    val.realValue = (double)value.x;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.y;
    jsonDataArrayAddValue(data, array, val);
}

Vec2 jsonDataGetVec2(const JsonData* data, uint32_t vector)
{
    const JsonObject* object = jsonDataGetChildConst(data, vector);
    Vec2 vec;
    
    vec.x = getJsonReal(object, 0);
    vec.y = getJsonReal(object, 1);
    
    return vec;
}

//Vec3
void jsonDataAddVec3(JsonData* data, uint32_t parent, Tag name, Vec3 value)
{
    JsonValue val;
    uint32_t array = jsonDataAddArray(data, parent, name);
    
    val.type = JSON_TYPE_REAL;
    val.realValue = (double)value.x;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.y;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.z;
    jsonDataArrayAddValue(data, array, val);
}

void jsonDataArrayAddVec3(JsonData* data, uint32_t parent, Vec3 value)
{
    JsonValue val;
    uint32_t array = jsonDataArrayAddArray(data, parent);
    
    val.type = JSON_TYPE_REAL;
    val.realValue = (double)value.x;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.y;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.z;
    jsonDataArrayAddValue(data, array, val);
}

Vec3 jsonDataGetVec3(const JsonData* data, uint32_t vector)
{
    const JsonObject* object = jsonDataGetChildConst(data, vector);
    Vec3 vec;
    double out;
    
    vec.x = getJsonReal(object, 0);
    vec.y = getJsonReal(object, 1);
    vec.z = getJsonReal(object, 2);
    
    return vec;
}

//Vec4
void jsonDataAddVec4(JsonData* data, uint32_t parent, Tag name, Vec4 value)
{
    JsonValue val;
    uint32_t array = jsonDataAddArray(data, parent, name);
    
    val.type = JSON_TYPE_REAL;
    val.realValue = (double)value.x;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.y;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.z;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.w;
    jsonDataArrayAddValue(data, array, val);
}

void jsonDataArrayAddVec4(JsonData* data, uint32_t parent, Vec4 value)
{
    JsonValue val;
    uint32_t array = jsonDataArrayAddArray(data, parent);
    
    val.type = JSON_TYPE_REAL;
    val.realValue = (double)value.x;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.y;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.z;
    jsonDataArrayAddValue(data, array, val);
    
    val.realValue = (double)value.w;
    jsonDataArrayAddValue(data, array, val);
}

Vec4 jsonDataGetVec4(const JsonData* data, uint32_t vector)
{
    const JsonObject* object = jsonDataGetChildConst(data, vector);
    Vec4 vec;
    
    vec.x = getJsonReal(object, 0);
    vec.y = getJsonReal(object, 1);
    vec.z = getJsonReal(object, 2);
    vec.w = getJsonReal(object, 3);
    
    return vec;
}

//Mat4
void jsonDataAddMat4(JsonData* data, uint32_t parent, Tag name, const Mat4* value)
{
    JsonValue val;
    uint32_t array = jsonDataAddArray(data, parent, name);
    
    val.type = JSON_TYPE_REAL;
    for(uint32_t i = 0; i < 16; ++i)
    {
        val.realValue = (double)value->data[i];
        jsonDataArrayAddValue(data, array, val);
    }
}

void jsonDataArrayAddMat4(JsonData* data, uint32_t parent, const Mat4* value)
{
    JsonValue val;
    uint32_t array = jsonDataArrayAddArray(data, parent);
    
    val.type = JSON_TYPE_REAL;
    for(uint32_t i = 0; i < 16; ++i)
    {
        val.realValue = (double)value->data[i];
        jsonDataArrayAddValue(data, array, val);
    }
}

void jsonDataGetMat4(const JsonData* data, uint32_t matrix, Mat4* outValue)
{
    const JsonObject* object = jsonDataGetChildConst(data, matrix);
    
    for(uint32_t i = 0; i < 16; ++i)
    {
        outValue->data[i] = getJsonReal(object, i);
    }
}
