#ifndef SPRITE_COMPONENT
#define SPRITE_COMPONENT

#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "core/tag.h"
#include "render/texture.h"

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
}
SpriteComponent;

COLLECTION_DECL(SpriteComponent)
MWQUEUE_DECL(SpriteComponent)
ECTCOLUMN_DECL(SpriteComponent)

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //SPRITE_COMPONENT
