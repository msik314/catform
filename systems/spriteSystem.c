#include "util/globalDefs.h"
#include "systems/spriteSystem.h"

#include <string.h>

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/sceneManager.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "ecs/phase.h"
#include "systems/systems.h"
#include "util/atomics.h"

const JobDependency SPRITE_READY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(SpriteComponent), PHASE_PARENT)}};
const JobDependency SPRITE_COPY_DEPS = {1, {MAKE_JOB_ID(SYSTEM(SpriteComponent), PHASE_UPDATE)}};

const JobDependency SPRITE_DESTROY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(SpriteComponent), PHASE_MARK)}};

static void spriteColCreate(ECTColumn* column){ectColumnCreate(SpriteComponent)((ECTColumn(SpriteComponent)*)column);}

const ECSystem SPRITE_SYSTEM =
{
    spriteSysInit,
    spriteSysDestroy,
    
    spriteColCreate,
    
    spriteCompReady, &SPRITE_READY_DEPS,
    spriteSysUpdate,
    spriteCompCopy, &SPRITE_COPY_DEPS,
    spriteCompDestroy, &SPRITE_DESTROY_DEPS,
    
    spriteCompReadyAll,
    spriteCompDestroyAll
};

void spriteSysInit(ECSystem* self){}
void spriteSysDestroy(ECSystem* self){}

void spriteCompReady(ECSystem* self, ECTColumn* column)
{
    ECTColumn(SpriteComponent)* sprites = (ECTColumn(SpriteComponent)*)column;
    uint32_t idx = sprites->components.size;
    uint32_t flags;
    
    while(idx)
    {
        --idx;
        
        flags = atomicLoad32(&sprites->components.data[idx].self.flags);
        if(!(flags & OBJECT_FLAG_UNREADY))
        {
            return;
        }
        
        fetchAnd32(&sprites->components.data[idx].self.flags, ~OBJECT_FLAG_UNREADY);
        spriteReady(&sprites->components.data[idx]);
    }
}

void spriteSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime){}

void spriteCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime){}

void spriteCompDestroy(ECSystem* self, ECTColumn* column)
{
    ECTColumn(SpriteComponent)* sprites = (ECTColumn(SpriteComponent)*)column;
    uint32_t flags;
    
    for(uint32_t i = 0; i < sprites->components.size; ++i)
    {
        flags = atomicLoad32(&sprites->components.data[i].self.flags);
        if(flags & OBJECT_FLAG_REMOVE) spriteOnDestroy(&sprites->components.data[i]);
    }
}

void spriteCompReadyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(SpriteComponent)* sprites = (ECTColumn(SpriteComponent)*)column;
    
    for(uint32_t i = 0; i < sprites->components.size; ++i)
    {
        fetchAnd32(&sprites->components.data[i].self.flags, ~OBJECT_FLAG_UNREADY);
        spriteReady(&sprites->components.data[i]);
    }
}

void spriteCompDestroyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(SpriteComponent)* sprites = (ECTColumn(SpriteComponent)*)column;
    SpriteComponent sprite;
    
    for(uint32_t i = 0; i < sprites->components.size; ++i)
    {
        spriteOnDestroy(&sprites->components.data[i]);
    }
    
    while(mwQueuePop(SpriteComponent)(&sprites->addQueue, &sprite))
    {
        spriteOnDestroy(&sprite);
    }
}

void spriteReady(SpriteComponent* sprite){}
void spriteOnDestroy(SpriteComponent* sprite){} 
