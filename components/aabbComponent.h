#ifndef AABB_COMPONENT_H
#define AABB_COMPONENT_H

#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "cmath/cvec.h"
#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    Object self;
    Vec2 offset;
    Vec2 size;
}
AabbComponent;

COLLECTION_DECL(AabbComponent)
MWQUEUE_DECL(AabbComponent)
ECTCOLUMN_DECL(AabbComponent)

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //AABB_COMPONENT_H
