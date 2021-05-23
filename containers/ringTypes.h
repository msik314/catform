#ifndef RING_TYPES_H
#define RING_TYPES_H

#include <stdint.h>
#include "containers/ring.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

RING_DECL(uint8_t)

RING_DECL(uint16_t)

RING_DECL(uint32_t)

RING_DECL(uint64_t)

RING_DECL(int8_t)

RING_DECL(int16_t)

RING_DECL(int32_t)

RING_DECL(int64_t)

RING_DECL(char)

RING_DECL(bool)

RING_DECL(float)

RING_DECL(double)

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //RING_TYPES_H
