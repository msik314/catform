#ifndef SHADER_SRC_H
#define SHADER_SRC_H

#include <stdint.h>
#include <GL/gl3w.h>
#include "cmath/cVec.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

extern const char* VERT_SRC;
extern const char* FRAG_SRC;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SHADER_SRC_H
