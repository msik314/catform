#include "util/globalDefs.h"
#include "containers/ring.h"

#include <stdint.h>

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

#define RING_IMPL(TYPE)\
\
void ringCreate(TYPE)(Ring(TYPE)* ring)\
{\
    ring->data = NULL;\
    ring->head = 0;\
    ring->size = 0;\
    ring->capacity = 0;\
}\
\
void ringDestroy(TYPE)(Ring(TYPE)* ring)\
{\
    if(ring->data) CAT_FREE(ring->data);\
}\
\
void ringAdd(TYPE)(Ring(TYPE)* ring, const TYPE* original)\
{\
    uint32_t capacity;\
    if(ring->size == ring->capacity)\
    {\
        capacity = ring->capacity + ring->capacity + (ring->capacity == 0);\
        TYPE* temp = (TYPE*)CAT_MALLOC(capacity * sizeof(TYPE));\
        for(uint32_t i = 0; i < ring->size; ++i)\
        {\
            temp[i] = ring->data[(ring->head + i) & (ring->capacity - 1)];\
        }\
        \
        if(ring->data) CAT_FREE(ring->data);\
        ring->data = temp;\
        ring->head = 0;\
        ring->capacity = capacity;\
    }\
    ring->data[(ring->head + ring->size) & (ring->capacity - 1)] = *original;\
    ++ring->size;\
}\
\
int32_t ringPop(TYPE)(Ring(TYPE)* ring)\
{\
    if(!ring->size) return 0;\
    \
    ring->head = (ring->head + 1) & (ring->capacity - 1);\
    --ring->size;\
    return 1;\
}\
\
void ringClear(TYPE)(Ring(TYPE)* ring)\
{\
    CAT_FREE(ring->data);\
    ring->data = NULL;\
    ring->head = 0;\
    ring->size = 0;\
    ring->capacity = 0;\
}\
\
int32_t ringGet(TYPE)(Ring(TYPE)* ring, TYPE* out)\
{\
    if(!ring->size) return 0;\
    \
    *out = ring->data[ring->head];\
    return 1;\
}

#include "containers/ringImpls.h"
