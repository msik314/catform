#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include "cmath/cvec.h"
#include "cmath/transform.h"
#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "core/tag.h"
#include "util/atomics.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    Object self;
    Transform transform;
    volatile uint64_t componentMask[2];
    Tag name;
}
Entity;

static inline bool entityHasComponent(const Entity* entity, uint32_t index)
{
    return (entity->componentMask[0] & (1ull << index)) || (entity->componentMask[1] & (1ull << (index - 64)));
}

static inline void entitySetHasComponent(Entity* entity, uint32_t index)
{
    if(index < 64)
    {
        fetchOr64(&entity->componentMask[0], 1ull << index);
    }
    else
    {
        fetchOr64(&entity->componentMask[1], 1ull << (index - 64));
    }
}

static inline void entityResetHasComponent(Entity* entity, uint32_t index)
{
    if(index < 64)
    {
        fetchAnd64(&entity->componentMask[0], ~(1ull << index));
    }
    else
    {
        fetchAnd64(&entity->componentMask[1], ~(1ull << (index - 64)));
    }
}

COLLECTION_DECL(Entity)
MWQUEUE_DECL(Entity)
ECTCOLUMN_DECL(Entity)

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //ENTITY_H
