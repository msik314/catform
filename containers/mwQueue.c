#include "util/globalDefs.h"
#include "containers/mwQueue.h"

#include <stdint.h>
#include "util/atomics.h"
#include "util/utilMacros.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

#define QUEUE_POOL_MIN 8
#define QUEUE_POOL_MIN_LOG2 3
#define QUEUE_STALE_DELAY 5

void mwQueueDestroyGeneric(MWQueue* queue)
{
    for(uint32_t i = 0; i < QUEUE_POOL_LEVELS; ++i)
    {
        if(queue->pool[i]) CAT_FREE((void*)(queue->pool[i]));
    }
}

#define MWQUEUE_IMPL(TYPE)\
void mwQueueCreate(TYPE)(MWQueue(TYPE)* queue)\
{\
    queue->pool[0] = (volatile TYPE*)CAT_MALLOC(sizeof(TYPE) * QUEUE_POOL_MIN);\
    for(uint32_t i = 1; i < QUEUE_POOL_LEVELS; ++i)\
    {\
        queue->pool[i] = NULL;\
    }\
    atomicStore32(&queue->size, 0);\
    atomicStore32(&queue->index, 0 );\
    atomicStore32(&queue->growCounter, 0);\
    atomicStore32(&queue->staleCounter, 0);\
}\
\
void mwQueueDestroy(TYPE)(MWQueue(TYPE)* queue)\
{\
    for(uint32_t i = 0; i < QUEUE_POOL_LEVELS; ++i)\
    {\
        if(queue->pool[i]) CAT_FREE((void*)(queue->pool[i]));\
    }\
}\
\
void mwQueuePush(TYPE)(MWQueue(TYPE)* queue, TYPE* original)\
{\
    volatile TYPE* poolLevel;\
    TYPE* newPool;\
    int32_t level;\
    int32_t poolIndex;\
    uint32_t index = fetchAdd32(&queue->size, 1);\
    bool res = false;\
\
    level = lastSet(index + QUEUE_POOL_MIN);\
    poolIndex = index - (1 << level) + QUEUE_POOL_MIN;\
    level -= QUEUE_POOL_MIN_LOG2;\
\
    poolLevel = (TYPE*)atomicLoad64(&queue->pool[level]);\
    if(!poolLevel)\
    {\
        newPool = (TYPE*) CAT_MALLOC(sizeof(TYPE) * (QUEUE_POOL_MIN << level));\
        do\
        {\
            res = compareAndSwap64(&queue->pool[level], &poolLevel, newPool);\
        }\
        while(!res && !poolLevel);\
\
        if(res)\
        {\
            poolLevel = newPool;\
            fetchAdd32(&queue->growCounter, 1);\
        }\
        else\
        {\
            CAT_FREE(newPool);\
        }\
    }\
\
    poolLevel[poolIndex] = *original;\
}\
\
static inline void shrinkQueue ## TYPE(MWQueue(TYPE)* queue)\
{\
    void* temp;\
    for(int32_t i = 1; i < QUEUE_POOL_LEVELS - 1; ++i)\
    {\
        if(!queue->pool[i]) return;\
        if(queue->pool[i] && !queue->pool[i + 1])\
        {\
            do\
            {\
                temp = (void*)queue->pool[i];\
                compareAndSwap64(&queue->pool[i], &temp, NULL);\
            }\
            while(queue->pool[i] != NULL);\
            CAT_FREE(temp);\
        }\
    }\
    do\
    {\
        temp = (void*)queue->pool[QUEUE_POOL_LEVELS - 1];\
        compareAndSwap64(&queue->pool[QUEUE_POOL_LEVELS - 1], &temp, NULL);\
    }\
    while(queue->pool[QUEUE_POOL_LEVELS - 1] != NULL);\
    CAT_FREE(temp);\
}\
\
bool mwQueuePop(TYPE)(MWQueue(TYPE)* queue, TYPE* out)\
{\
    int32_t level;\
    int32_t poolIndex;\
    int32_t index;\
    if(queue->size == queue->index)\
    {\
        if(queue->growCounter)\
        {\
            atomicStore32(&queue->growCounter, 0);\
            atomicStore32(&queue->staleCounter, 0);\
        }\
        else\
        {\
            if(fetchAdd32(&queue->staleCounter, 1) >= QUEUE_STALE_DELAY - 1)\
            {\
                shrinkQueue ## TYPE(queue);\
                atomicStore32(&queue->staleCounter, 0);\
            }\
        }\
        atomicStore32(&queue->size, 0);\
        atomicStore32(&queue->index, 0);\
        return false;\
    }\
\
    index = fetchAdd32(&queue->index, 1);\
\
    level = lastSet(index + QUEUE_POOL_MIN);\
    poolIndex = index - (1 << level) + QUEUE_POOL_MIN;\
    level -= QUEUE_POOL_MIN_LOG2;\
    *out = queue->pool[level][poolIndex];\
    return true;\
}

#include "containers/mwQueueImpls.h"
