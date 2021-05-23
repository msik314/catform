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

COLLECTION_DECL(uint8_t)

COLLECTION_DECL(uint16_t)

COLLECTION_DECL(uint32_t)

COLLECTION_DECL(uint64_t)

COLLECTION_DECL(int8_t)

COLLECTION_DECL(int16_t)

COLLECTION_DECL(int32_t)

COLLECTION_DECL(int64_t)

COLLECTION_DECL(char)

COLLECTION_DECL(bool)

COLLECTION_DECL(float)

COLLECTION_DECL(double)

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //COLLECTION_TYPES_H
