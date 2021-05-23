#ifndef VECTOR_TYPES_H
#define VECTOR_TYPES_H

#include <stdint.h>
#include "containers/vector.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

VECTOR_DECL(uint8_t)

VECTOR_DECL(uint16_t)

VECTOR_DECL(uint32_t)

VECTOR_DECL(uint64_t)

VECTOR_DECL(int8_t)

VECTOR_DECL(int16_t)

VECTOR_DECL(int32_t)

VECTOR_DECL(int64_t)

VECTOR_DECL(char)

VECTOR_DECL(bool)

VECTOR_DECL(float)

VECTOR_DECL(double)

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //VECTOR_TYPES_H
