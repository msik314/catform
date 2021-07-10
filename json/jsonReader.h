#ifndef JSON_READER_H
#define JSON_READER_H

#include <string.h>
#include "json/jsonData.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

static inline void jsonLoads(JsonData* data, const char* str, uint32_t len)
{
    jsonDataCreate(data);
    jsonDataParse(data, str, len);
}

static inline void jsonLoadc(JsonData* data, const char* cStr){jsonLoads(data, cStr, strlen(cStr));}

void jsonLoadf(JsonData* data, const char* fileName);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //JSON_READER_H
