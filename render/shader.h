#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>
#include <GL/gl3w.h>
#include "cmath/cVec.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef GLuint Shader;

int32_t shaderCreate(Shader* shader, const char* vertSrc, const char* fragSrc);
void shaderDestroy(Shader* shader);

static inline void bind(Shader* shader){glUseProgram(*shader);}

static inline void uniform1i(int32_t location, int32_t value){glUniform1i(location, value);}
static inline void uniform1u(int32_t location, uint32_t value){glUniform1ui(location, value);}
static inline void uniform1f(int32_t location, float value){glUniform1f(location, value);}
static inline void uniform2f(int32_t location, Vec2 value){glUniform2f(location, value.x, value.y);}
static inline void uniform3f(int32_t location, Vec3 value){glUniform3f(location, value.x, value.y, value.z);}
static inline void uniform4f(int32_t location, Vec4 value){glUniform4f(location, value.x, value.y, value.z, value.w);}

static inline void uniformMat2(int32_t location, const Mat2* value){glUniformMatrix2fv(location, 1, false, value->data);}
static inline void uniformMat3(int32_t location, const Mat3* value){glUniformMatrix3fv(location, 1, false, value->data);}
static inline void uniformMat4(int32_t location, const Mat4* value){glUniformMatrix4fv(location, 1, false, value->data);}

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SHADER_H
