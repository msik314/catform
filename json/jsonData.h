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
    Tag tag = {};
    tagSet(&tag, key);
    return tag;
}

static inline Tag jsonKeyLen(const char* key, uint32_t len)
{
    Tag tag = {};
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

JsonType jsonObjectGetKey(const JsonObject* object, const char* key, void* outVal);
static inline uint32_t jsonObjectSize(const JsonObject* object) {return object->object.size;}
JsonType jsonArrayGetIndex(const JsonObject* object, uint32_t index, void* outVal);
static inline uint32_t jsonArraySize(const JsonObject* object) {return object->array.size;}
static inline uint32_t jsonStringLen(const JsonObject* object) {return stringLength(&object->string);}

double jsonObjectGetKeyAsReal(const JsonObject* object, const char* key);
double jsonArrayGetIndexAsReal(const JsonObject* object, uint32_t index);

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
static inline const JsonObject* jsonDataGetChildConst(const JsonData* data, uint32_t index){return &data->children.data[index];}

void jsonDataAddBool(JsonData* data, uint32_t parent, Tag name, bool value);
void jsonDataAddInt(JsonData* data, uint32_t parent, Tag name, int32_t value);
void jsonDataAddReal(JsonData* data, uint32_t parent, Tag name, double value);
void jsonDataAddString(JsonData* data, uint32_t parent, Tag name, const char* value);
void jsonDataAddTag(JsonData* data, uint32_t parent, Tag name, Tag value);
void jsonDataAddValue(JsonData* data, uint32_t parent, Tag name, JsonValue value);
void jsonDataAddNull(JsonData* data, uint32_t parent, Tag name);

uint32_t jsonDataAddObject(JsonData* data, uint32_t parent, Tag name);
uint32_t jsonDataAddArray(JsonData* data, uint32_t parent, Tag name);


void jsonDataArrayAddBool(JsonData* data, uint32_t parentArray, bool value);
void jsonDataArrayAddInt(JsonData* data, uint32_t parentArray, int32_t value);
void jsonDataArrayAddReal(JsonData* data, uint32_t parentArray, double value);
void jsonDataArrayAddString(JsonData* data, uint32_t parentArray, const char* value);
void jsonDataArrayAddTag(JsonData* data, uint32_t parent, Tag value);
void jsonDataArrayAddValue(JsonData* data, uint32_t parentArray, JsonValue value);
void jsonDataArrayAddNull(JsonData* data, uint32_t parentArray);

uint32_t jsonDataArrayAddObject(JsonData* data, uint32_t parent);
uint32_t jsonDataArrayAddArray(JsonData* data, uint32_t parent);


#ifdef __cplusplus
};
#endif //__cplusplus

#endif //JSON_DATA_H
