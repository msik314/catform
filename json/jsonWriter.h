#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include <stdint.h>
#include "json/jsonData.h"
#include "containers/string.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

int32_t jsonWrites(const JsonData* data, const char* str, int32_t len);

void jsonWritef(const JsonData* data, const char* fileName);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //JSON_WRITER_H
