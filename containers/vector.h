#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    void* data;
    uint32_t size;
    uint32_t capacity;
}
Vector;

#define Vector(TYPE) Vector ## TYPE

#define vectorCreate(TYPE) vector ## TYPE ## Create
#define vectorDestroy(TYPE) vector ## TYPE ## Destroy
#define vectorAdd(TYPE) vector ## TYPE ## Add
#define vectorInsert(TYPE) vector ## TYPE ## INSERT
#define vectorRemove(TYPE) vector ## TYPE ## Remove
#define vectorPopN(TYPE) vector ## TYPE ## PopN
#define vectorPop(TYPE) vector ## TYPE ## Pop
#define vectorClear(TYPE) vector ## TYPE ## Clear
#define vectorResize(TYPE) vector ## TYPE ## Resize
#define vectorReserve(TYPE) vector ## TYPE ## Reserve

#define VECTOR_DECL(TYPE) \
typedef struct\
{\
    TYPE* data;\
    uint32_t size;\
    uint32_t capacity;\
}\
Vector(TYPE);\
\
void vectorCreate(TYPE)(Vector(TYPE)* vector);\
void vectorDestroy(TYPE)(Vector(TYPE)* vector);\
void vectorAdd(TYPE)(Vector(TYPE)* vector, const TYPE* original);\
void vectorInsert(TYPE)(Vector(TYPE)* vector, uint32_t index, const TYPE* original);\
void vectorRemove(TYPE)(Vector(TYPE)* vector, uint32_t index);\
void vectorPopN(TYPE)(Vector(TYPE)* vector, uint32_t amt);\
void vectorPop(TYPE)(Vector(TYPE)* vector);\
void vectorClear(TYPE)(Vector(TYPE)* vector);\
void vectorResize(TYPE)(Vector(TYPE)* vector, uint32_t size);\
void vectorReserve(TYPE)(Vector(TYPE)* vector, uint32_t capacity);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //VECTOR_H
