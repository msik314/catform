#include "util/globalDefs.h"
#include "systems/bulletSystem.h"

#include "core/input.h"
#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/sceneManager.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "ecs/phase.h"
#include "systems/systems.h"
#include "systems/entitySystem.h"
#include "systems/aabbSystem.h"
#include "util/linalloc.h"

const JobDependency BULLET_READY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(BulletComponent), PHASE_PARENT)}};
const JobDependency BULLET_COPY_DEPS = {2, {MAKE_JOB_ID(SYSTEM(BulletComponent), PHASE_UPDATE), MAKE_JOB_ID(SYSTEM(AabbComponent), PHASE_UPDATE)}};

const JobDependency BULLET_DESTROY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(BulletComponent), PHASE_MARK)}};

static void bulletColCreate(ECTColumn* column){ectColumnCreate(BulletComponent)((ECTColumn(BulletComponent)*)column);}

const ECSystem BULLET_SYSTEM =
{
    bulletSysInit,
    bulletSysDestroy,
    
    bulletColCreate,
    
    bulletCompReady, &BULLET_READY_DEPS,
    bulletSysUpdate,
    bulletCompCopy, &BULLET_COPY_DEPS,
    bulletCompDestroy, &BULLET_DESTROY_DEPS,
    
    bulletCompReadyAll,
    bulletCompDestroyAll
};

void bulletSysInit(ECSystem* self){}
void bulletSysDestroy(ECSystem* self){}

void bulletCompReady(ECSystem* self, ECTColumn* column)
{
    ECTColumn(BulletComponent)* bullets = (ECTColumn(BulletComponent)*)column;
    uint32_t idx = bullets->components.size;
    uint32_t flags;
    
    while(idx)
    {
        --idx;
        
        flags = atomicLoad32(&bullets->components.data[idx].self.flags);
        if(!(flags & OBJECT_FLAG_UNREADY))
        {
            return;
        }
        
        fetchAnd32(&bullets->components.data[idx].self.flags, ~OBJECT_FLAG_UNREADY);
        bulletReady(&bullets->components.data[idx]);
    }
}

void bulletSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime)
{
    const BulletComponent* bullets = getComponentsConst(columns, BulletComponent);
    uint32_t numBullets = getNumComponents(columns, BulletComponent);
    EntityMoveFlags* moveFlags = linalloc(OFFSETOF(EntityMoveFlags, updates) + numBullets * sizeof(EntityMove));
    uint32_t flagIdx = 0;
    const Input* input = inputGetInstance();
    register Vec2 movement;
    register float xInp;
    register float dTarget;
    register bool jumpInp;
    
    moveFlags->numUpdates = numBullets;
    
    for(uint32_t i = 0; i < numBullets; ++i)
    {
        moveFlags->updates[i].delta.x = bullets[i].velocity.x * deltaTime;
        moveFlags->updates[i].delta.y = bullets[i].velocity.y * deltaTime;
        moveFlags->updates[i].parent = bullets[i].self.parent;
    }
    
    *flags = moveFlags;
}

void bulletCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime)
{
    BulletComponent* bullets = (BulletComponent*)column->components.data;
    uint32_t numBullets = column->components.size;
    const AabbFlags* aabbFlags = (const AabbFlags*)atomicLoadPtr(&flags[SYSTEM(AabbComponent)]);
    SceneManager* sceneMan = sceneManagerGetInstance();
    ECTable* table = sceneManagerGetTable(sceneMan);
    const PointerMap* map = sceneManagerGetMap(sceneMan);
    const Entity* enitities = getComponentsConst(table->columns, Entity);
    
    register uint32_t idx;
    
    for(uint32_t i = 0; i < numBullets; ++i)
    {
        bullets[i].fallDelay -= deltaTime;
        bullets[i].lifeTime -= deltaTime;
        if(bullets[i].lifeTime <= 0.0f)
        {
            ecTableRemove(table, Entity, bullets[i].self.parent);
        }
        else if(bullets[i].fallDelay <= 0.0f)
        {
            bullets[i].velocity.y += bullets[i].gravity * deltaTime;
        }
    }
    
    for(uint32_t i = 0; i < aabbFlags->numCollisions; ++i)
    {
        idx = pointerMapGet(map, aabbFlags->collisions[i].entity1);
        if(entityHasComponent(&enitities[idx], COMPONENT(BulletComponent)))
        {
            ecTableRemove(table, Entity, aabbFlags->collisions[i].entity1);
        }
        
        idx = pointerMapGet(map, aabbFlags->collisions[i].entity2);
        if(entityHasComponent(&enitities[idx], COMPONENT(BulletComponent)))
        {
            ecTableRemove(table, Entity, aabbFlags->collisions[i].entity2);
        }
    }
}

void bulletCompDestroy(ECSystem* self, ECTColumn* column)
{
    ECTColumn(BulletComponent)* bullets = (ECTColumn(BulletComponent)*)column;
    uint32_t flags;
    
    for(uint32_t i = 0; i < bullets->components.size; ++i)
    {
        flags = atomicLoad32(&bullets->components.data[i].self.flags);
        if(flags & OBJECT_FLAG_REMOVE) bulletOnDestroy(&bullets->components.data[i]);
    }
}

void bulletCompReadyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(BulletComponent)* bullets = (ECTColumn(BulletComponent)*)column;
    
    for(uint32_t i = 0; i < bullets->components.size; ++i)
    {
        fetchAnd32(&bullets->components.data[i].self.flags, ~OBJECT_FLAG_UNREADY);
        bulletReady(&bullets->components.data[i]);
    }
}

void bulletCompDestroyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(BulletComponent)* bullets = (ECTColumn(BulletComponent)*)column;
    BulletComponent bullet;
    
    for(uint32_t i = 0; i < bullets->components.size; ++i)
    {
        bulletOnDestroy(&bullets->components.data[i]);
    }
    
    while(mwQueuePop(BulletComponent)(&bullets->addQueue, &bullet))
    {
        bulletOnDestroy(&bullet);
    }
}

void bulletReady(BulletComponent* bullet){}

void bulletOnDestroy(BulletComponent* bullet){} 
