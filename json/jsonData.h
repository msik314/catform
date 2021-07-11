#ifndef JSON_DATA_H
#define JSON_DATA_H

#include <stdint.h>
#include <string.h>
#include "core/tag.h"
#include "containers/vector.h"
#include "containers/vectorTypes.h"
#include "containers/hashmap.h"
#include "containers/string.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define JSON_DATA_ROOT_INDEX 0xffffffff

static inline Tag jsonKey(const char* key)
{
    Tag tag;
    strncpy(tag.data, key, sizeof(tag.data));
    return tag;
}

static inline Tag jsonKeyLen(const char* key, uint32_t len)
{
    Tag tag;
    memset(tag.data, 0, sizeof(tag.data));
    memcpy(tag.data, key, len);
    return tag;
}

typedef enum
{
    JSON_TYPE_UNKNOWN,
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_STRING,
    JSON_TYPE_NULL,
    JSON_TYPE_BOOL,
    JSON_TYPE_REAL,
    JSON_TYPE_INT
}
JsonType;

typedef struct
{
    union
    {
        bool boolValue;
        int32_t intValue;
        double realValue;
        char* stringValue;
        uint32_t objectIndex;
    };
    JsonType type;
}
JsonValue;

HASHMAP_DECL(Tag, JsonValue)
VECTOR_DECL(JsonValue)

typedef union
{
    Hashmap(Tag, JsonValue) object;
    Vector(JsonValue) array;
    String string;
}
JsonObject;

JsonType jsonObjectGetKey(JsonObject* object, const char* key, void* outVal);
static inline uint32_t jsonObjectSize(JsonObject* object) {return object->object.size;}
JsonType jsonArrayGetIndex(JsonObject* object, uint32_t index, void* outVal);
static inline uint32_t jsonArraySize(JsonObject* object) {return object->array.size;}
static inline uint32_t jsonStringLen(JsonObject* object) {return stringLength(&object->string);}

VECTOR_DECL(JsonObject);

typedef struct
{
    JsonObject root;
    Vector(JsonObject) children;
}
JsonData;

void jsonDataCreate(JsonData* data);
int32_t jsonDataParse(JsonData* data, const char* str, uint32_t len);
void jsonDataDestroy(JsonData* data);
static inline JsonObject* jsonDataGetChild(JsonData* data, uint32_t index){return &data->children.data[index];}

void jsonDataAddValue(JsonData* data, uint32_t parent, Tag name, JsonValue value);
uint32_t jsonDataAddObject(JsonData* data, uint32_t parent, Tag name);
uint32_t jsonDataAddArray(JsonData* data, uint32_t parent, Tag name);


void jsonDataArrayAddValue(JsonData* data, uint32_t parentArray, JsonValue value);
uint32_t jsonDataArrayAddObject(JsonData* data, uint32_t parent);
uint32_t jsonDataArrayAddArray(JsonData* data, uint32_t parent);


#ifdef __cplusplus
};
#endif //__cplusplus

#endif //JSON_DATA_H
