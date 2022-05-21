#include "util/globalDefs.h"
#include "json/jsonWriter.h"

#include <stdint.h>
#include <stdio.h>
#include <stb/stb_sprintf.h>

#include "util/utilMacros.h"
#include "core/tag.h"
#include "containers/hashmap.h"
#include "json/jsonData.h"

static int32_t writeArrayString(const JsonData* data, const JsonObject* object, char* output, int32_t bufferSize);
static int32_t writeObjectString(const JsonData* data, const JsonObject* object, char* output, int32_t bufferSize);

static void writeArrayFile(const JsonData* data, const JsonObject* object, FILE* output);
static void writeObjectFile(const JsonData* data, const JsonObject* object, FILE* output);

static inline int32_t snputc(char c, char* str, int32_t len)
{
    if(len < 1) return 0;
    
    str[0] = c;
    return 1;
}

int32_t jsonWrites(const JsonData* data, const char* str, int32_t len)
{
    int32_t written = writeObjectString(data, &data->root, (char*)str, len);
    written += snputc('\0', (char*)str + written, len - written);
    
    return written;
}

void jsonWritef(const JsonData* data, const char* fileName)
{
    FILE* file = fopen(fileName, "w");
    
    writeObjectFile(data, &data->root, file);
    
    fclose(file);
}

static int32_t writeArrayString(const JsonData* data, const JsonObject* object, char* output, int32_t bufferSize)
{
    const JsonObject* child = NULL;
    JsonValue value;
    int32_t remaining = bufferSize;
    int32_t res = 0;
    char tag[sizeof(Tag) + 1] = {};
    
    res = snputc('[', output, remaining);
    remaining -= res;
    output += res;
    
    for(uint32_t i = 0; i < jsonArraySize(object); ++i)
    {
        if(i) 
        {
            res = snputc(',', output, remaining);
            remaining -= res;
            output += res;
        }
        
        value.type = jsonArrayGetIndex(object, i, &value);
        
        switch(value.type)
        {
        case JSON_TYPE_OBJECT:
            child = jsonDataGetChildConst(data, value.objectIndex);
            res = writeObjectString(data, child, output, remaining);
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_ARRAY:
            child = jsonDataGetChildConst(data, value.objectIndex);
            res = writeArrayString(data, child, output, remaining);
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_STRING:
            res = stbsp_snprintf(output, MAX(remaining, 0), "\"%s\"", value.stringValue);
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_BOOL:
            res = stbsp_snprintf(output, MAX(remaining, 0), "%s", value.boolValue ? "true" : "false");
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_REAL:
            res = stbsp_snprintf(output, MAX(remaining, 0), "%f", value.realValue);
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_INT:
            res = stbsp_snprintf(output, MAX(remaining, 0), "%d", value.intValue);
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_NULL:
            res = stbsp_snprintf(output, MAX(remaining, 0), "null");
            remaining -= res;
            output += res;
        
        default:
            res = stbsp_snprintf(output, MAX(remaining, 0), "undefined");
            remaining -= res;
            output += res;
        }
    }
    
    res = snputc(']', output, remaining);
    remaining -= res;
    output += res;
    return bufferSize - remaining;
}

static int32_t writeObjectString(const JsonData* data, const JsonObject* object, char* output, int32_t bufferSize)
{
    HashmapIterator(Tag, JsonValue) itr = hashmapBegin(Tag, JsonValue)(&object->object);
    const JsonObject* child = NULL;
    JsonValue value;
    int32_t remaining = bufferSize;
    int32_t res = 0;
    char tag[sizeof(Tag) + 1] = {};
    
    if(!jsonObjectSize(object))
    {
        res = snputc('{', output, remaining);
        remaining -= res;
        output += res;
        
        res = snputc('}', output, remaining);
        remaining -= res;
        output += res;
        
        return bufferSize - remaining;
    }
    
    res = snputc('{', output, remaining);
    remaining -= res;
    output += res;
    
    while(1)
    {
        hashmapIteratorKey(Tag, JsonValue)(&itr, (Tag*)tag);
        hashmapIteratorValue(Tag, JsonValue)(&itr, &value);
        
        switch(value.type)
        {
        case JSON_TYPE_OBJECT:
            res = stbsp_snprintf(output, MAX(remaining, 0), "\"%s\":", tag);
            remaining -= res;
            output += res;
            child = jsonDataGetChildConst(data, value.objectIndex);
            res = writeObjectString(data, child, output, remaining);
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_ARRAY:
            res = stbsp_snprintf(output, MAX(remaining, 0), "\"%s\":", tag);
            remaining -= res;
            output += res;
            child = jsonDataGetChildConst(data, value.objectIndex);
            res = writeArrayString(data, child, output, remaining);
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_STRING:
            res = stbsp_snprintf(output, MAX(remaining, 0), "\"%s\":\"%s\"", tag, value.stringValue);
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_BOOL:
            res = stbsp_snprintf(output, MAX(remaining, 0), "\"%s\":%s", tag, value.boolValue ? "true" : "false");
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_REAL:
            res = stbsp_snprintf(output, MAX(remaining, 0), "\"%s\":%f", tag, value.realValue);
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_INT:
            res = stbsp_snprintf(output, MAX(remaining, 0), "\"%s\":%d", tag, value.intValue);
            remaining -= res;
            output += res;
            break;
            
        case JSON_TYPE_NULL:
            res = stbsp_snprintf(output, MAX(remaining, 0), "\"%s\":null", tag);
            remaining -= res;
            output += res;
            break;
        
        default:
            res = stbsp_snprintf(output, MAX(remaining, 0), "\"%s\":undefined", tag);
            remaining -= res;
            output += res;
        }
        
        if(!hashmapIteratorNext(Tag, JsonValue)(&itr)) break;
        
        res = snputc(',', output, remaining);
        remaining -= res;
        output += res;
    }
    
    res = snputc('}', output, remaining);
    remaining -= res;
    output += res;
    return bufferSize - remaining;
}

static void writeArrayFile(const JsonData* data, const JsonObject* object, FILE* output)
{
    const JsonObject* child = NULL;
    JsonValue value;
    fputc('[', output);
    
    for(uint32_t i = 0; i < jsonArraySize(object); ++i)
    {
        if(i) fputc(',', output);
        
        value.type = jsonArrayGetIndex(object, i, &value);
        switch(value.type)
        {
        case JSON_TYPE_OBJECT:
            child = jsonDataGetChildConst(data, value.objectIndex);
            writeObjectFile(data, child, output);
            break;
            
        case JSON_TYPE_ARRAY:
            child = jsonDataGetChildConst(data, value.objectIndex);
            writeArrayFile(data, child, output);
            break;
            
        case JSON_TYPE_STRING:
            fprintf(output, "\"%s\"", value.stringValue);
            break;
            
        case JSON_TYPE_BOOL:
            fprintf(output, "%s", value.boolValue ? "true" : "false");
            break;
            
        case JSON_TYPE_REAL:
            fprintf(output, "%f", value.realValue);
            break;
            
        case JSON_TYPE_INT:
            fprintf(output, "%d", value.intValue);
            break;
            
        case JSON_TYPE_NULL:
        default:
            fprintf(output, "null");
        }
    }
    fputc(']', output);
}

static void writeObjectFile(const JsonData* data, const JsonObject* object, FILE* output)
{
    HashmapIterator(Tag, JsonValue) itr = hashmapBegin(Tag, JsonValue)(&object->object);
    const JsonObject* child = NULL;
    JsonValue value;
    char tag[sizeof(Tag) + 1] = {};
    
    if(!jsonObjectSize(object))
    {
        fputc('{', output);
        fputc('}', output);
        return;
    }
    
    fputc('{', output);
    
    while(1)
    {
        hashmapIteratorKey(Tag, JsonValue)(&itr, (Tag*)tag);
        hashmapIteratorValue(Tag, JsonValue)(&itr, &value);
        
        switch(value.type)
        {
        case JSON_TYPE_OBJECT:
            fprintf(output, "\"%s\":", tag);
            child = jsonDataGetChildConst(data, value.objectIndex);
            writeObjectFile(data, child, output);
            break;
            
        case JSON_TYPE_ARRAY:
            fprintf(output, "\"%s\":", tag);
            child = jsonDataGetChildConst(data, value.objectIndex);
            writeArrayFile(data, child, output);
            break;
            
        case JSON_TYPE_STRING:
            fprintf(output, "\"%s\":\"%s\"", tag, value.stringValue);
            break;
            
        case JSON_TYPE_BOOL:
            fprintf(output, "\"%s\":%s", tag, value.boolValue ? "true" : "false");
            break;
            
        case JSON_TYPE_REAL:
            fprintf(output, "\"%s\":%f", tag, value.realValue);
            break;
            
        case JSON_TYPE_INT:
            fprintf(output, "\"%s\":%d", tag, value.intValue);
            break;
            
        case JSON_TYPE_NULL:
        default:
            fprintf(output, "\"%s\":null", tag);
        }
        
        if(!hashmapIteratorNext(Tag, JsonValue)(&itr)) break;
        
        fputc(',', output);
    }
    fputc('}', output);
}
