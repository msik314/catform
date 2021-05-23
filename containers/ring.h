#ifndef RING_H 
#define RING_H

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
    uint32_t head;
    uint32_t size;
    uint32_t capacity;
}
Ring;

#define Ring(TYPE) Ring ## TYPE

#define ringCreate(TYPE) ring ## TYPE ## Create
#define ringDestroy(TYPE) ring ## TYPE ## Destroy
#define ringAdd(TYPE) ring ## TYPE ## Add
#define ringGet(TYPE) ring ## TYPE ## Get
#define ringPop(TYPE) ring ## TYPE ## Pop
#define ringClear(TYPE) ring ## TYPE ## Clear
#define ringSize(TYPE) ring ## TYPE ## Size

#define RING_DECL(TYPE) \
typedef struct\
{\
    TYPE* data;\
    uint32_t head;\
    uint32_t size;\
    uint32_t capacity;\
}\
Ring(TYPE);\
\
void ringCreate(TYPE)(Ring(TYPE)* ring);\
void ringDestroy(TYPE)(Ring(TYPE)* ring);\
void ringAdd(TYPE)(Ring(TYPE)* ring, const TYPE* original);\
int32_t ringPop(TYPE)(Ring(TYPE)* ring);\
int32_t ringGet(TYPE)(Ring(TYPE)* ring, TYPE* out);\
void ringClear(TYPE)(Ring(TYPE)* ring);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //RING_H
