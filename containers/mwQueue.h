#ifndef MW_QUEUE_H 
#define MW_QUEUE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define QUEUE_POOL_LEVELS 4

typedef struct
{
    volatile void* pool[QUEUE_POOL_LEVELS];
    volatile uint32_t size;
    volatile uint32_t index;
    volatile uint32_t growCounter;
    volatile uint32_t staleCounter;
}
MWQueue;

void mwQueueDestroyGeneric(MWQueue* queue);

#define MWQueue(TYPE) MWQueue ## TYPE

#define mwQueueCreate(TYPE) mwQueue ## TYPE ## Create
#define mwQueueDestroy(TYPE) mwQueue ## TYPE ## Destroy
#define mwQueuePush(TYPE) mwQueue ## TYPE ## Push
#define mwQueuePop(TYPE) mwQueue ## TYPE ## Pop

#define MWQUEUE_DECL(TYPE) \
typedef struct\
{\
    volatile TYPE* pool[QUEUE_POOL_LEVELS];\
    volatile uint32_t size;\
    volatile uint32_t index;\
    volatile uint32_t growCounter;\
    volatile uint32_t staleCounter;\
}\
MWQueue(TYPE);\
\
void mwQueueCreate(TYPE)(MWQueue(TYPE)* queue);\
void mwQueueDestroy(TYPE)(MWQueue(TYPE)* queue);\
void mwQueuePush(TYPE)(MWQueue(TYPE)* queue, TYPE* original);\
bool mwQueuePop(TYPE)(MWQueue(TYPE)* queue, TYPE* out);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //MW_QUEUE_H
