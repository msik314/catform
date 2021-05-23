#ifndef COLLECTION_TYPES_H
#define COLLECTION_TYPES_H

#include <stdint.h>
#include "containers/collection.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

MWQUEUE_DECL(uint8_t)

MWQUEUE_DECL(uint16_t)

MWQUEUE_DECL(uint32_t)

MWQUEUE_DECL(uint64_t)

MWQUEUE_DECL(int8_t)

MWQUEUE_DECL(int16_t)

MWQUEUE_DECL(int32_t)

MWQUEUE_DECL(int64_t)

MWQUEUE_DECL(char)

MWQUEUE_DECL(bool)

MWQUEUE_DECL(float)

MWQUEUE_DECL(double)

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //COLLECTION_TYPES_H
