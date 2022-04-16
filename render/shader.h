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

static inline void shaderBind(Shader* shader){glUseProgram(*shader);}

static inline void shaderUniform1i(int32_t location, int32_t value){glUniform1i(location, value);}
static inline void shaderUniform1u(int32_t location, uint32_t value){glUniform1ui(location, value);}
static inline void shaderUniform1f(int32_t location, float value){glUniform1f(location, value);}
static inline void shaderUniform2f(int32_t location, Vec2 value){glUniform2f(location, value.x, value.y);}
static inline void shaderUniform3f(int32_t location, Vec3 value){glUniform3f(location, value.x, value.y, value.z);}
static inline void shaderUniform4f(int32_t location, Vec4 value){glUniform4f(location, value.x, value.y, value.z, value.w);}

static inline void shaderUniformMat2(int32_t location, const Mat2* value){glUniformMatrix2fv(location, 1, false, value->data);}
static inline void shaderUniformMat3(int32_t location, const Mat3* value){glUniformMatrix3fv(location, 1, false, value->data);}
static inline void shaderUniformMat4(int32_t location, const Mat4* value){glUniformMatrix4fv(location, 1, false, value->data);}

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SHADER_H
