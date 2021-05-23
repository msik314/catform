#include "util/globalDefs.h"
#include "containers/vector.h"
#include <stdint.h>

#include "util/utilMacros.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

#define VECTOR_IMPL(TYPE)\
\
void vectorCreate(TYPE)(Vector(TYPE)* vector)\
{\
    vector->data = NULL;\
    vector->size = 0;\
    vector->capacity = 0;\
}\
\
void vectorDestroy(TYPE)(Vector(TYPE)* vector)\
{\
    if(vector->data) CAT_FREE(vector->data);\
}\
\
void vectorAdd(TYPE)(Vector(TYPE)* vector, const TYPE* original)\
{\
    if(vector->size == vector->capacity)\
    {\
        vector->capacity = vector->capacity + vector->capacity + (vector->capacity == 0);\
        TYPE* temp = (TYPE*)CAT_MALLOC(vector->capacity * sizeof(TYPE));\
        for(uint32_t i = 0; i < vector->size; ++i)\
        {\
            temp[i] = vector->data[i];\
        }\
        if(vector->data) CAT_FREE(vector->data);\
        vector->data = temp;\
    }\
    vector->data[vector->size++] = *original;\
}\
\
void vectorInsert(TYPE)(Vector(TYPE)* vector, uint32_t index, const TYPE* original)\
{\
    if(vector->size == vector->capacity)\
    {\
        vector->capacity = vector->capacity + vector->capacity + (vector->capacity == 0);\
        TYPE* temp = (TYPE*)CAT_MALLOC(vector->capacity * sizeof(TYPE));\
        for(uint32_t i = 0; i < index; ++i)\
        {\
            temp[i] = vector->data[i];\
        }\
        \
        for(uint32_t i = index; i < vector->size; ++i)\
        {\
            temp[i + 1] = vector->data[i];\
        }\
        temp[index] = *original;\
        if(vector->data) CAT_FREE(vector->data);\
        vector->data = temp;\
    }\
    else\
    {\
        for(uint32_t i = vector->size; i > index; --i)\
        {\
            vector->data[i] = vector->data[i - 1];\
        }\
        vector->data[index] = *original;\
    }\
    ++(vector->size);\
}\
\
void vectorRemove(TYPE)(Vector(TYPE)* vector, uint32_t index)\
{\
    --(vector->size);\
    for(uint32_t i = index; i < vector->size; ++i)\
    {\
        vector->data[i] = vector->data[i + 1];\
    }\
}\
\
void vectorPopN(TYPE)(Vector(TYPE)* vector, uint32_t amt)\
{\
    vector->size -= amt;\
}\
\
void vectorPop(TYPE)(Vector(TYPE)* vector)\
{\
   --(vector->size);\
}\
\
void vectorClear(TYPE)(Vector(TYPE)* vector)\
{\
    CAT_FREE(vector->data);\
    vector->data = NULL;\
    vector->size = 0;\
    vector->capacity = 0;\
}\
\
void vectorResize(TYPE)(Vector(TYPE)* vector, uint32_t size)\
{\
    if(size > vector->capacity) vectorReserve(TYPE)(vector, 1 << (lastSet(size - 1) + 1));\
    vector->size = size;\
}\
void vectorReserve(TYPE)(Vector(TYPE)* vector, uint32_t capacity)\
{\
    if(vector->capacity > capacity) return;\
    \
    vector->capacity = capacity;\
    TYPE* temp = (TYPE*)CAT_MALLOC(vector->capacity * sizeof(TYPE));\
    for(uint32_t i = 0; i < vector->size; ++i)\
    {\
        temp[i] = vector->data[i];\
    }\
    if(vector->data) CAT_FREE(vector->data);\
    vector->data = temp;\
}

#include "containers/vectorImpls.h"
