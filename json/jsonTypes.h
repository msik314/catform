#ifndef JSON_TYPES_H
#define JSON_TYPES_H

#include "json/jsonData.h"
#include "cmath/cvec.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

void jsonDataAddVec2(JsonData* data, uint32_t parent, Tag name, Vec2 value);
void jsonDataArrayAddVec2(JsonData* data, uint32_t parent, Vec2 value);
Vec2 jsonDataGetVec2(const JsonData* data, uint32_t vector);

void jsonDataAddVec3(JsonData* data, uint32_t parent, Tag name, Vec3 value);
void jsonDataArrayAddVec3(JsonData* data, uint32_t parent, Vec3 value);
Vec3 jsonDataGetVec3(const JsonData* data, uint32_t vector);

void jsonDataAddVec4(JsonData* data, uint32_t parent, Tag name, Vec4 value);
void jsonDataArrayAddVec4(JsonData* data, uint32_t parent, Vec4 value);
Vec4 jsonDataGetVec4(const JsonData* data, uint32_t vector);

#define jsonDataAddQuat(data, parent, name, value) jsonDataAddVec4(data, parent, name, value)
#define jsonDataArrayAddQuat(data, parent, value) jsonDataArrayAddVec4(data, parent, value)
#define jsonDataGetQuat(data, vector) jsonDataGetVec4(data, vector)

void jsonDataAddMat4(JsonData* data, uint32_t parent, Tag name, const Mat4* value);
void jsonDataArrayAddMat4(JsonData* data, uint32_t parent, const Mat4* value);
void jsonDataGetMat4(const JsonData* data, uint32_t matrix, Mat4* outValue);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //JSON_TYPES_H
