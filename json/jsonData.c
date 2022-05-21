#include "util/globalDefs.h"
#include "json/jsonData.h"

#include <stdlib.h>
#include <string.h>
#define JSMN_PARENT_LINKS
#include <jsmn/jsmn.h>
#include "core/error.h"
#include "core/tag.h"
#include "util/utilMacros.h"

#ifndef CAT_MALLOC
#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

static JsonValue jsonParseToken(JsonData* data, const char* string, const jsmntok_t* tokens, int32_t* tokenIdx, int32_t maxIdx);

JsonType jsonObjectGetKey(const JsonObject* object, const char* key, void* outVal)
{
    Tag tag = jsonKey(key);
    JsonValue value;
    uint32_t res;
    
    res = hashmapGet(Tag, JsonValue)(&object->object, &tag, &value);
    if(!res) return JSON_TYPE_UNKNOWN;
    
    switch(value.type)
    {
    case JSON_TYPE_OBJECT:
    case JSON_TYPE_ARRAY:
        *(uint32_t*)outVal = value.objectIndex;
        break;
        
    case JSON_TYPE_STRING:
        *(char**)outVal = value.stringValue;
        break;
        
    case JSON_TYPE_NULL:
        *(uintptr_t*)outVal = 0;
        break;
        
    case JSON_TYPE_BOOL:
        *(bool*)outVal = value.boolValue;
        break;
        
    case JSON_TYPE_REAL:
        *(double*)outVal = value.realValue;
        break;
        
    case JSON_TYPE_INT:
        *(int32_t*)outVal = value.intValue;
        break;
        
    default:
        break;
    }
    
    return value.type;
}

double jsonObjectGetKeyAsReal(const JsonObject* object, const char* key)
{
    Tag tag = jsonKey(key);
    JsonValue value;
    uint32_t res;
    
    res = hashmapGet(Tag, JsonValue)(&object->object, &tag, &value);
    if(!res) return JSON_TYPE_UNKNOWN;
    
    switch(value.type)
    {
    case JSON_TYPE_REAL:
        return value.realValue;
        
    case JSON_TYPE_INT:
        return (double)value.intValue;
        
    default:
        return 0.0;
    }
}

JsonType jsonArrayGetIndex(const JsonObject* object, uint32_t index, void* outVal)
{
    switch(object->array.data[index].type)
    {
    case JSON_TYPE_OBJECT:
    case JSON_TYPE_ARRAY:
        *(uint32_t*)outVal = object->array.data[index].objectIndex;
        break;
        
    case JSON_TYPE_STRING:
        *(char**)outVal = object->array.data[index].stringValue;
        break;
        
    case JSON_TYPE_NULL:
        *(uintptr_t*)outVal = 0;
        break;
        
    case JSON_TYPE_BOOL:
        *(bool*)outVal = object->array.data[index].boolValue;
        break;
        
    case JSON_TYPE_REAL:
        *(double*)outVal = object->array.data[index].realValue;
        break;
        
    case JSON_TYPE_INT:
        *(int32_t*)outVal = object->array.data[index].intValue;
        break;
        
    default:
        break;
    }
    
    return object->array.data[index].type;
}

double jsonArrayGetIndexAsReal(const JsonObject* object, uint32_t index)
{
    switch(object->array.data[index].type)
    {
    case JSON_TYPE_REAL:
        return object->array.data[index].realValue;
        
    case JSON_TYPE_INT:
        return (double)(object->array.data[index].intValue);
        
    default:
        return 0.0;
    }
    
    return object->array.data[index].type;
}

void jsonDataCreate(JsonData* data)
{
    hashmapCreate(Tag, JsonValue)(&data->root.object);
    vectorCreate(JsonObject)(&data->children);
}

JsonValue jsonParseObject(JsonData* data, const char* string, const jsmntok_t* tokens, int32_t* tokenIdx, int32_t maxIdx)
{
    JsonValue value;
    JsonObject object = {};
    Tag tag;
    int32_t arrayIdx;
    uint32_t tagLen;
    
    hashmapCreate(Tag, JsonValue)(&object.object);
    
    arrayIdx = (*tokenIdx)++;
    
    while(*tokenIdx < maxIdx && tokens[*tokenIdx].parent == arrayIdx)
    {
        tagLen = tokens[*tokenIdx].end - tokens[*tokenIdx].start;
        tag = jsonKeyLen(&string[tokens[*tokenIdx].start], MIN(tagLen, sizeof(Tag)));
        
        ++(*tokenIdx);
        
        value = jsonParseToken(data, string, tokens, tokenIdx, maxIdx);
        if(value.type == JSON_TYPE_UNKNOWN)
        {
            vectorAdd(JsonObject)(&data->children, &object);
            return value;
        }
        
        hashmapSet(Tag, JsonValue)(&object.object, &tag, &value);
    }
    
    value.type = JSON_TYPE_OBJECT;
    value.objectIndex = data->children.size;
    vectorAdd(JsonObject)(&data->children, &object);
    return value;
}

JsonValue jsonParsePrimitive(const char* string, const jsmntok_t* tokens, int32_t* tokenIdx)
{
    JsonValue value;
    uint32_t tokenLen = tokens[*tokenIdx].end - tokens[*tokenIdx].start;
    
    switch (string[tokens[*tokenIdx].start])
    {
    case 'n':
        value.stringValue = NULL;
        value.type = JSON_TYPE_NULL;
        break;
    case 't':
        value.boolValue = true;
        value.type = JSON_TYPE_BOOL;
        break;
    case 'f':
        value.boolValue = false;
        value.type = JSON_TYPE_BOOL;
        break;
    default:
        if(memchr(&string[tokens[*tokenIdx].start], '.', tokenLen))
        {
            value.realValue = atof(&string[tokens[*tokenIdx].start]);
            value.type = JSON_TYPE_REAL;
        }
        else
        {
            value.intValue = atoi(&string[tokens[*tokenIdx].start]);
            value.type = JSON_TYPE_INT;
        }
        break;
    }
    
    ++(*tokenIdx);
    return value;
}

JsonValue jsonParseArray(JsonData* data, const char* string, const jsmntok_t* tokens, int32_t* tokenIdx, int32_t maxIdx)
{
    JsonValue value;
    JsonObject object = {};
    int32_t arrayIdx;
    
    vectorCreate(JsonValue)(&object.array);
    arrayIdx = (*tokenIdx)++;
    
    while(*tokenIdx < maxIdx && tokens[*tokenIdx].parent == arrayIdx)
    {
        value = jsonParseToken(data, string, tokens, tokenIdx, maxIdx);
        if(value.type == JSON_TYPE_UNKNOWN)
        {
            vectorAdd(JsonObject)(&data->children, &object);
            return value;
        }
        
        vectorAdd(JsonValue)(&object.array, &value);
    }
    
    value.type = JSON_TYPE_ARRAY;
    value.objectIndex = data->children.size;
    vectorAdd(JsonObject)(&data->children, &object);
    return value;
}

JsonValue jsonParseString(JsonData* data, const char* string, const jsmntok_t* tokens, int32_t* tokenIdx)
{
    JsonValue value;
    JsonObject object = {};
    uint32_t strLen;
    
    strLen = tokens[*tokenIdx].end - tokens[*tokenIdx].start;
    stringCreate(&object.string);
    strSliceToString(&object.string, &string[tokens[*tokenIdx].start], strLen);    
    
    value.type = JSON_TYPE_STRING;
    value.stringValue = object.string.data;
    vectorAdd(JsonObject)(&data->children, &object);
    
    ++(*tokenIdx);
    
    return value;
}

static JsonValue jsonParseToken(JsonData* data, const char* string, const jsmntok_t* tokens, int32_t* tokenIdx, int32_t maxIdx)
{
    uint32_t errorLen;
    char* errorStr;
    JsonValue val;
    uint32_t tokenLen;
    
    switch(tokens[*tokenIdx].type)
    {
    case JSMN_OBJECT:
        return jsonParseObject(data, string, tokens, tokenIdx, maxIdx);
        
    case JSMN_PRIMITIVE:
        return jsonParsePrimitive(string, tokens, tokenIdx);
    
    case JSMN_ARRAY:
        return jsonParseArray(data, string, tokens, tokenIdx, maxIdx);
    
    case JSMN_STRING:
        return jsonParseString(data, string, tokens, tokenIdx);
    
    default:
        tokenLen = tokens[*tokenIdx].end - tokens[*tokenIdx].start;
        errorLen = strlen("unrecognized json type ");
        errorStr = (char*)CAT_MALLOC(errorLen + tokenLen + 3);
        strcpy(errorStr, "unrecognized json type ");
        errorStr[errorLen++] = '0' + tokens[*tokenIdx].type;
        errorStr[errorLen++] = ' ';
        memcpy(&errorStr[errorLen], &string[tokens[*tokenIdx].start], tokenLen);
        errorLen += tokenLen;
        errorStr[errorLen] = 0;
        
        printErr(CAT_ERROR_JSON, errorStr);
        break;
    }
    
    val.type = JSON_TYPE_UNKNOWN;
    return val;
}

int32_t jsonDataParse(JsonData* data, const char* str, uint32_t len)
{
    jsmn_parser jsmnp;
    jsmntok_t* tokens;
    JsonValue value;
    Tag tag;
    uint32_t tagLen;
    int32_t numTokens;
    int32_t tokenIdx = 0;
    
    hashmapClear(Tag, JsonValue)(&data->root.object);
    for(int32_t i = 0; i < data->children.size; ++i)
    {
        //Vector and hashmap have the same internal structure, and both just need to free their array
        vectorDestroy(JsonValue)(&data->children.data[i].array);
    }
    vectorClear(JsonObject)(&data->children);
    
    jsmn_init(&jsmnp);
    
    numTokens = jsmn_parse(&jsmnp, str, len, NULL, 0xffffffff);
    if(numTokens < 0)
    {
        printErr(CAT_ERROR_JSON, "invalid json string");
        return CAT_ERROR_JSON;
    }
    
    tokens = (jsmntok_t*)CAT_MALLOC(numTokens * (sizeof(jsmntok_t) + sizeof(int32_t)));
    
    jsmn_init(&jsmnp);
    numTokens = jsmn_parse(&jsmnp, str, len, tokens, numTokens);
    
    ++tokenIdx;
    
    while(tokenIdx < numTokens && tokens[tokenIdx].parent == 0)
    {
        tagLen = tokens[tokenIdx].end - tokens[tokenIdx].start;
        tag = jsonKeyLen(&str[tokens[tokenIdx].start], MIN(tagLen, sizeof(Tag)));
        
        ++tokenIdx;
        
        value = jsonParseToken(data, str, tokens, &tokenIdx, numTokens);
        if(value.type == JSON_TYPE_UNKNOWN)
        {
            CAT_FREE(tokens);
            return CAT_ERROR_JSON;
        }
        
        hashmapSet(Tag, JsonValue)(&data->root.object, &tag, &value);
    }
    
    CAT_FREE(tokens);
    return CAT_SUCCESS;
}

void jsonDataDestroy(JsonData* data)
{
    hashmapDestroy(Tag, JsonValue)(&data->root.object);
    
    for(int32_t i = 0; i < data->children.size; ++i)
    {
        //Vector and hashmap have the same internal structure, and both just need to free their array
        vectorDestroy(JsonValue)(&data->children.data[i].array);
    }
    
    vectorDestroy(JsonObject)(&data->children);
}


void jsonDataAddValuePtr(JsonData* data, uint32_t parent, Tag name, JsonValue* value)
{
    JsonObject* parentPtr;
    JsonObject* stringVal;
    JsonValue val;
    
    JsonObject obj = {};
    
    switch(value->type)
    {
    case JSON_TYPE_ARRAY:
    case JSON_TYPE_OBJECT:
        printWarn(CAT_WARNING_JSON, "jsonDataAddValue should only be used for primitives.  Use jsonDataAddObject jsonDataAddArray for objects or arrays");
        return;
        
    case JSON_TYPE_UNKNOWN:
        printWarn(CAT_WARNING_JSON, "cannot add JSON value with unknown type");
        return;
        
    case JSON_TYPE_STRING:
        vectorAdd(JsonObject)(&data->children, &obj);
        stringVal = jsonDataGetChild(data, data->children.size - 1);
        stringCreate(&stringVal->string);
        cStrToString(&stringVal->string, value->stringValue);
        
        val.stringValue = stringVal->string.data;
        val.type = JSON_TYPE_STRING;
        parentPtr = parent != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parent) : &data->root;
        hashmapSet(Tag, JsonValue)(&parentPtr->object, &name, &val);
        break;
    
    default:
        parentPtr = parent != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parent) : &data->root;
        hashmapSet(Tag, JsonValue)(&parentPtr->object, &name, value);
    }
}

void jsonDataAddValue(JsonData* data, uint32_t parent, Tag name, JsonValue value) {jsonDataAddValuePtr(data, parent, name, &value);}

void jsonDataAddBool(JsonData* data, uint32_t parent, Tag name, bool value)
{
    JsonValue val;
    val.type = JSON_TYPE_BOOL;
    val.boolValue = value;
    jsonDataAddValuePtr(data, parent, name, &val);
}

void jsonDataAddInt(JsonData* data, uint32_t parent, Tag name, int32_t value)
{
    JsonValue val;
    val.type = JSON_TYPE_INT;
    val.intValue = value;
    jsonDataAddValuePtr(data, parent, name, &val);
}

void jsonDataAddReal(JsonData* data, uint32_t parent, Tag name, double value)
{
    JsonValue val;
    val.type = JSON_TYPE_REAL;
    val.realValue = value;
    jsonDataAddValuePtr(data, parent, name, &val);
}

void jsonDataAddString(JsonData* data, uint32_t parent, Tag name, const char* value)
{
    JsonValue val;
    val.type = JSON_TYPE_STRING;
    val.stringValue = (char*)value;
    jsonDataAddValuePtr(data, parent, name, &val);
}

void jsonDataAddNull(JsonData* data, uint32_t parent, Tag name)
{
    JsonValue val;
    val.type = JSON_TYPE_NULL;
    val.stringValue = NULL;
    jsonDataAddValuePtr(data, parent, name, &val);
}

void jsonDataAddTag(JsonData* data, uint32_t parent, Tag name, Tag value)
{
    JsonObject* parentPtr;
    JsonObject* stringVal;
    JsonValue val;
    
    JsonObject obj = {};
    
    vectorAdd(JsonObject)(&data->children, &obj);
    stringVal = jsonDataGetChild(data, data->children.size - 1);
    stringCreate(&stringVal->string);
    strSliceToString(&stringVal->string, value.data, sizeof(Tag));
    
    val.stringValue = stringVal->string.data;
    val.type = JSON_TYPE_STRING;
    parentPtr = parent != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parent) : &data->root;
    hashmapSet(Tag, JsonValue)(&parentPtr->object, &name, &val);
}


uint32_t jsonDataAddObject(JsonData* data, uint32_t parent, Tag name)
{
    JsonObject object;
    JsonObject* parentPtr;
    JsonValue val;
    uint32_t idx;
    
    val.type = JSON_TYPE_OBJECT;
    hashmapCreate(Tag, JsonValue)(&object.object);
    
    vectorAdd(JsonObject)(&data->children, &object);
    idx = data->children.size - 1;
    val.objectIndex = idx;
    parentPtr = parent != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parent) : &data->root;
    hashmapSet(Tag, JsonValue)(&parentPtr->object, &name, &val);
    
    return idx;
}

uint32_t jsonDataAddArray(JsonData* data, uint32_t parent, Tag name)
{
    JsonObject object;
    JsonObject* parentPtr;
    JsonValue val;
    uint32_t idx;
    
    val.type = JSON_TYPE_ARRAY;
    vectorCreate(JsonValue)(&object.array);
    
    vectorAdd(JsonObject)(&data->children, &object);
    idx = data->children.size - 1;
    val.objectIndex = idx;
    parentPtr = parent != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parent) : &data->root;
    hashmapSet(Tag, JsonValue)(&parentPtr->object, &name, &val);
    
    return idx;
}

static void jsonDataArrayAddValuePtr(JsonData* data, uint32_t parentArray, JsonValue* value)
{
    JsonObject* parentPtr;
    JsonObject* stringVal;
    JsonValue val;
    
    JsonObject obj = {};
    
    switch(value->type)
    {
    case JSON_TYPE_ARRAY:
    case JSON_TYPE_OBJECT:
        printWarn(CAT_WARNING_JSON, "jsonDataArrayAddValue should only be used for primitives.  jsonDataArrayAddObject jsonDataArrayAddArray for objects or arrays");
        return;
        
    case JSON_TYPE_UNKNOWN:
        printWarn(CAT_WARNING_JSON, "cannot add JSON value with unknown type");
        return;
        
    case JSON_TYPE_STRING:
        vectorAdd(JsonObject)(&data->children, &obj);
        stringVal = jsonDataGetChild(data, data->children.size - 1);
        stringCreate(&stringVal->string);
        cStrToString(&stringVal->string, value->stringValue);
        
        val.stringValue = stringVal->string.data;
        val.type = JSON_TYPE_STRING;
        parentPtr = parentArray != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parentArray) : &data->root;
        vectorAdd(JsonValue)(&parentPtr->array, &val);
        break;
    
    default:
        parentPtr = parentArray != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parentArray) : &data->root;
        vectorAdd(JsonValue)(&parentPtr->array, value);
    }
}

void jsonDataArrayAddValue(JsonData* data, uint32_t parentArray, JsonValue value) {jsonDataArrayAddValuePtr(data, parentArray, &value);}

void jsonDataArrayAddBool(JsonData* data, uint32_t parentArray, bool value)
{
    JsonValue val;
    val.type = JSON_TYPE_BOOL;
    val.boolValue = value;
    jsonDataArrayAddValuePtr(data, parentArray, &val);
}

void jsonDataArrayAddInt(JsonData* data, uint32_t parentArray, int32_t value)
{
    JsonValue val;
    val.type = JSON_TYPE_INT;
    val.intValue = value;
    jsonDataArrayAddValuePtr(data, parentArray, &val);
}

void jsonDataArrayAddReal(JsonData* data, uint32_t parentArray, double value)
{
    JsonValue val;
    val.type = JSON_TYPE_REAL;
    val.realValue = value;
    jsonDataArrayAddValuePtr(data, parentArray, &val);
}

void jsonDataArrayAddString(JsonData* data, uint32_t parentArray, const char* value)
{
    JsonValue val;
    val.type = JSON_TYPE_STRING;
    val.stringValue = (char*)value;
    jsonDataArrayAddValuePtr(data, parentArray, &val);
}

void jsonDataArrayAddNull(JsonData* data, uint32_t parentArray)
{
    JsonValue val;
    val.type = JSON_TYPE_NULL;
    val.stringValue = NULL;
    jsonDataArrayAddValuePtr(data, parentArray, &val);
}

void jsonDataArrayAddTag(JsonData* data, uint32_t parentArray, Tag value)
{
    JsonObject* parentPtr;
    JsonObject* stringVal;
    JsonValue val;
    
    JsonObject obj = {};
    
    vectorAdd(JsonObject)(&data->children, &obj);
    stringVal = jsonDataGetChild(data, data->children.size - 1);
    stringCreate(&stringVal->string);
    strSliceToString(&stringVal->string, value.data, sizeof(Tag));
    
    val.stringValue = stringVal->string.data;
    val.type = JSON_TYPE_STRING;
    parentPtr = parentArray != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parentArray) : &data->root;
    vectorAdd(JsonValue)(&parentPtr->array, &val);
}

uint32_t jsonDataArrayAddObject(JsonData* data, uint32_t parentArray)
{
    JsonObject object;
    JsonObject* parentPtr;
    JsonValue val;
    uint32_t idx;
    
    val.type = JSON_TYPE_OBJECT;
    hashmapCreate(Tag, JsonValue)(&object.object);
    
    vectorAdd(JsonObject)(&data->children, &object);
    idx = data->children.size - 1;
    val.objectIndex = idx;
    parentPtr = parentArray != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parentArray) : &data->root;
    vectorAdd(JsonValue)(&parentPtr->array, &val);
    
    return idx;
}

uint32_t jsonDataArrayAddArray(JsonData* data, uint32_t parentArray)
{
    JsonObject object;
    JsonObject* parentPtr;
    JsonValue val;
    uint32_t idx;
    
    val.type = JSON_TYPE_ARRAY;
    vectorCreate(JsonValue)(&object.array);
    
    vectorAdd(JsonObject)(&data->children, &object);
    idx = data->children.size - 1;
    val.objectIndex = idx;
    parentPtr = parentArray != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parentArray) : &data->root;
    vectorAdd(JsonValue)(&parentPtr->array, &val);
    
    return idx;
}
