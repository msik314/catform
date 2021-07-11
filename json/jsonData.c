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

#define INVALID_ARRAY_PARENT -1

#define UNRECOGNIZED_TYPE_STR "unrecognized json type "

JsonType jsonObjectGetKey(JsonObject* object, const char* key, void* outVal)
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

JsonType jsonArrayGetIndex(JsonObject* object, uint32_t index, void* outVal)
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

void jsonDataCreate(JsonData* data)
{
    hashmapCreate(Tag, JsonValue)(&data->root.object);
    vectorCreate(JsonObject)(&data->children);
}

int32_t jsonDataParse(JsonData* data, const char* str, uint32_t len)
{
    jsmn_parser jsmnp;
    jsmntok_t* tokens;
    int32_t* objectIndices;
    int32_t numTokens;
    uint32_t cpLen;
    uint32_t tokenLen;
    int32_t tokenIndex;
    Tag tag;
    JsonValue value;
    JsonObject object;
    JsonObject* parent;
    uint32_t arrayIdx = 0;
    int32_t arrayParent = INVALID_ARRAY_PARENT;
    char* errorStr;
    char terminator = 0;
    
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
    objectIndices = (int32_t*)(&tokens[numTokens]);
    
    jsmn_init(&jsmnp);
    numTokens = jsmn_parse(&jsmnp, str, len, tokens, numTokens);
    
    objectIndices[0] = -1;
    
    tokenIndex = 1;
    while(tokenIndex < numTokens)
    {
        if(tokens[tokenIndex].parent != arrayParent)
        {
            arrayParent = INVALID_ARRAY_PARENT;
            tokenLen = tokens[tokenIndex].end - tokens[tokenIndex].start;
            cpLen = MIN(tokenLen, sizeof(tag.data));
            tag = jsonKeyLen(&str[tokens[tokenIndex].start], cpLen);
            ++tokenIndex;
        }
        
        tokenLen = tokens[tokenIndex].end - tokens[tokenIndex].start;
        switch(tokens[tokenIndex].type)
        {
        case JSMN_OBJECT:
            hashmapCreate(Tag, JsonValue)(&object.object);
            vectorAdd(JsonObject)(&data->children, &object);
            objectIndices[tokenIndex] = data->children.size - 1;
            value.objectIndex = data->children.size - 1;
            value.type = JSON_TYPE_OBJECT;
            break;
            
        case JSMN_PRIMITIVE:
            switch (str[tokens[tokenIndex].start])
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
                if(memchr(&str[tokens[tokenIndex].start], '.', tokenLen))
                {
                    value.realValue = atof(&str[tokens[tokenIndex].start]);
                    value.type = JSON_TYPE_REAL;
                }
                else
                {
                    value.intValue = atoi(&str[tokens[tokenIndex].start]);
                    value.type = JSON_TYPE_INT;
                }
                break;
            }
            break;
            
        case JSMN_ARRAY:
            vectorCreate(JsonValue)(&object.array);
            vectorAdd(JsonObject)(&data->children, &object);
            objectIndices[tokenIndex] = data->children.size - 1;
            value.objectIndex = data->children.size - 1;
            value.type = JSON_TYPE_ARRAY;
            arrayParent = tokenIndex;
            arrayIdx = 0;
            break;
            
        case JSMN_STRING:
            vectorAdd(JsonObject)(&data->children, &object);
            value.objectIndex = data->children.size - 1;
            value.type = JSON_TYPE_STRING;
            strSliceToString(&data->children.data[value.objectIndex].string, &str[tokens[tokenIndex].start], tokenLen);
            value.stringValue = data->children.data[value.objectIndex].string.data;
            break;
            
        default:
            arrayIdx = strlen(UNRECOGNIZED_TYPE_STR);
            errorStr = CAT_MALLOC(arrayIdx + tokenLen + 3);
            strcpy(errorStr, UNRECOGNIZED_TYPE_STR);
            errorStr[arrayIdx++] = '0' + tokens[tokenIndex].type;
            errorStr[arrayIdx++] = ' ';
            memcpy(&errorStr[arrayIdx], &str[tokens[tokenIndex].start], tokenLen);
            arrayIdx += tokenLen;
            errorStr[arrayIdx] = 0;
            
            printErr(CAT_ERROR_JSON, errorStr);
            CAT_FREE(errorStr);
            CAT_FREE(tokens);
            return CAT_ERROR_JSON;
        }
        
        if(tokens[tokenIndex].parent == arrayParent)
        {
            parent = &data->children.data[objectIndices[arrayParent]];
            vectorAdd(JsonValue)(&parent->array, &value);
        }
        else if(tokens[tokenIndex - 1].parent == 0)
        {
            hashmapSet(Tag, JsonValue)(&data->root.object, &tag, &value);
        }
        else
        {
            parent = &data->children.data[objectIndices[tokens[tokenIndex - 1].parent]];
            hashmapSet(Tag, JsonValue)(&parent->object, &tag, &value);
        }
        
        ++tokenIndex;
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


void jsonDataAddValue(JsonData* data, uint32_t parent, Tag name, JsonValue value)
{
    JsonObject* parentPtr;
    JsonObject* stringVal;
    JsonValue val;
    
    JsonObject obj = {};
    
    switch(value.type)
    {
    case JSON_TYPE_ARRAY:
    case JSON_TYPE_OBJECT:
        printWarn(CAT_WARNING_JSON, "jsonDataAddValue should only be used for primitives.  Use jsonDataAddObject for arrays and object");
        return;
        
    case JSON_TYPE_UNKNOWN:
        printWarn(CAT_WARNING_JSON, "cannot add JSON value with unknown type");
        return;
        
    case JSON_TYPE_STRING:
        vectorAdd(JsonObject)(&data->children, &obj);
        stringVal = jsonDataGetChild(data, data->children.size - 1);
        stringCreate(&stringVal->string);
        cStrToString(&stringVal->string, value.stringValue);
        
        val.stringValue = stringVal->string.data;
        val.type = JSON_TYPE_STRING;
        parentPtr = parent != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parent) : &data->root;
        hashmapSet(Tag, JsonValue)(&parentPtr->object, &name, &val);
        break;
    
    default:
        parentPtr = parent != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parent) : &data->root;
        hashmapSet(Tag, JsonValue)(&parentPtr->object, &name, &value);
    }
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

void jsonDataArrayAddValue(JsonData* data, uint32_t parentArray, JsonValue value)
{
    JsonObject* parentPtr;
    JsonObject* stringVal;
    JsonValue val;
    
    JsonObject obj = {};
    
    switch(value.type)
    {
    case JSON_TYPE_ARRAY:
    case JSON_TYPE_OBJECT:
        printWarn(CAT_WARNING_JSON, "jsonDataAddValue should only be used for primitives.  Use jsonDataAddObject for arrays and object");
        return;
        
    case JSON_TYPE_UNKNOWN:
        printWarn(CAT_WARNING_JSON, "cannot add JSON value with unknown type");
        return;
        
    case JSON_TYPE_STRING:
        vectorAdd(JsonObject)(&data->children, &obj);
        stringVal = jsonDataGetChild(data, data->children.size - 1);
        stringCreate(&stringVal->string);
        cStrToString(&stringVal->string, value.stringValue);
        
        val.stringValue = stringVal->string.data;
        val.type = JSON_TYPE_STRING;
        parentPtr = parentArray != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parentArray) : &data->root;
        vectorAdd(JsonValue)(&parentPtr->array, &val);
        break;
    
    default:
        parentPtr = parentArray != JSON_DATA_ROOT_INDEX ? jsonDataGetChild(data, parentArray) : &data->root;
        vectorAdd(JsonValue)(&parentPtr->array, &value);
    }
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
