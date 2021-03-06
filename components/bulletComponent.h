#ifndef BULLET_COMPONENT_H
#define BULLET_COMPONENT_H

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
    Vec2 velocity;
    float gravity;
    float fallDelay;
    float damage;
    float knockBack;
    float lifeTime;
}
BulletComponent;

COLLECTION_DECL(BulletComponent)
MWQUEUE_DECL(BulletComponent)
ECTCOLUMN_DECL(BulletComponent)

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //BULLET_COMPONENT_H
