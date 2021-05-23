#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include "cmath/cvec.h"
#include "cmath/transform.h"
#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "ecs/ecTable.h"

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
}
Entity;

COLLECTION_DECL(Entity)
MWQUEUE_DECL(Entity)
ECTCOLUMN_DECL(Entity)
ECTABLE_OBJ_DECL(Entity);

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //ENTITY_H
