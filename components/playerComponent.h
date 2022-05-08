#ifndef PLAYER_COMPONENT_H
#define PLAYER_COMPONENT_H

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
    float moveSpeed;
    float jumpSpeed;
    float groundAccel;
    float airAccel;
    uint32_t controller1;
    uint32_t controller2;
    uint32_t vertical;
    uint32_t horizontal;
    uint32_t jumpBtn;
    Vec2 velocity;
    bool grounded;
}
PlayerComponent;

COLLECTION_DECL(PlayerComponent)
MWQUEUE_DECL(PlayerComponent)
ECTCOLUMN_DECL(PlayerComponent)

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //PLAYER_COMPONENT_H
