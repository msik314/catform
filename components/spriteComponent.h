#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "core/tag.h"
#include "render/texture.h"
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
    Texture texture;
    Tag texName;
    Vec4 tint;
}
SpriteComponent;

COLLECTION_DECL(SpriteComponent)
MWQUEUE_DECL(SpriteComponent)
ECTCOLUMN_DECL(SpriteComponent)

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //SPRITE_COMPONENT_H
