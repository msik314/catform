#include "util/globalDefs.h"
#include "systems/entitySystem.h"

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/sceneManager.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "ecs/phase.h"
#include "core/tag.h"
#include "components/entity.h"
#include "systems/systems.h"
#include "systems/aabbSystem.h"
#include "systems/playerSystem.h"
#include "util/atomics.h"

const JobDependency ENTITY_READY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(Entity), PHASE_PARENT)}};
const JobDependency ENTITY_COPY_DEPS = 
{
    4,
    {
        MAKE_JOB_ID(SYSTEM(Entity), PHASE_UPDATE),
        MAKE_JOB_ID(SYSTEM(PlayerComponent), PHASE_UPDATE),
        MAKE_JOB_ID(SYSTEM(AabbComponent), PHASE_UPDATE),
        MAKE_JOB_ID(SYSTEM(Render), PHASE_UPDATE)
    }
};

const JobDependency ENTITY_DESTROY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(Entity), PHASE_MARK)}};

static void entityColCreate(ECTColumn* column){ectColumnCreate(Entity)((ECTColumn(Entity)*)column);}

const ECSystem ENTITY_SYSTEM =
{
    entitySysInit,
    entitySysDestroy,
    
    entityColCreate,
    
    entityCompReady, &ENTITY_READY_DEPS,
    entitySysUpdate,
    entityCompCopy, &ENTITY_COPY_DEPS,
    entityCompDestroy, &ENTITY_DESTROY_DEPS,
    
    entityCompReadyAll,
    entityCompDestroyAll
};

void entitySysInit(ECSystem* self){}
void entitySysDestroy(ECSystem* self){}

void entityCompReady(ECSystem* self, ECTColumn* column)
{
    ECTColumn(Entity)* entities = (ECTColumn(Entity)*)column;
    uint32_t idx = entities->components.size;
    uint32_t flags;
    
    while(idx)
    {
        --idx;
        
        flags = atomicLoad32(&entities->components.data[idx].self.flags);
        if(!(flags & OBJECT_FLAG_UNREADY))
        {
            return;
        }
        
        fetchAnd32(&entities->components.data[idx].self.flags, ~OBJECT_FLAG_UNREADY);
        entityReady(&entities->components.data[idx]);
    }
}

void entitySysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime){}

static inline float vec2Dot(Vec2 lhs, Vec2 rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

void entityCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime)
{
    Entity* entities = (Entity*)column->components.data;
    uint32_t numEntities = column->components.size;
    const PlayerMoveFlags* playerFlags = (const PlayerMoveFlags*)atomicLoadPtr(&flags[SYSTEM(PlayerComponent)]);
    const EntityMoveFlags* bulletFlags = (const EntityMoveFlags*)atomicLoadPtr(&flags[SYSTEM(BulletComponent)]);
    const PointerMap* map = sceneManagerGetMap(sceneManagerGetInstance());
    const AabbFlags* collisions = flags[SYSTEM(AabbComponent)];
    Vec2 normals[numEntities];
    float overlaps[numEntities];
    register float amt;
    register uint32_t entityIdx;
    register Vec2 movement;
    
    for(uint32_t i = 0; i < numEntities; ++i)
    {
        normals[i].x = 0.0f;
        normals[i].y = 0.0f;
        overlaps[i] = 0;
    }
    
    for(uint32_t i = 0; i < collisions->numCollisions; ++i)
    {
        entityIdx = pointerMapGet(map, collisions->collisions[i].entity1);
        normals[entityIdx].x += collisions->collisions[i].normal.x;
        normals[entityIdx].y += collisions->collisions[i].normal.y;
        overlaps[entityIdx] += collisions->collisions[i].overlap;
        
        entityIdx = pointerMapGet(map, collisions->collisions[i].entity2);
        normals[entityIdx].x -= collisions->collisions[i].normal.x;
        normals[entityIdx].y -= collisions->collisions[i].normal.y;
        overlaps[entityIdx] += collisions->collisions[i].overlap;
    }
    
    for(uint32_t i = 0; i < playerFlags->numUpdates; ++i)
    {
        entityIdx = pointerMapGet(map, playerFlags->updates[i].parent);
        movement = playerFlags->updates[i].velocity;
        movement.x *= deltaTime;
        movement.y *= deltaTime;
        amt = vec2Dot(normals[entityIdx], movement);
        entities[entityIdx].transform.position.x += movement.x + (amt + overlaps[entityIdx]) * normals[entityIdx].x;
        entities[entityIdx].transform.position.y += movement.y + (amt + overlaps[entityIdx]) * normals[entityIdx].y;
    }
    
    for(uint32_t i = 0; i < bulletFlags->numUpdates; ++i)
    {
        entityIdx = pointerMapGet(map, bulletFlags->updates[i].parent);
        entities[entityIdx].transform.position.x += bulletFlags->updates[i].delta.x;
        entities[entityIdx].transform.position.y += bulletFlags->updates[i].delta.y;
    }
}

void entityCompDestroy(ECSystem* self, ECTColumn* column)
{
    ECTColumn(Entity)* entities = (ECTColumn(Entity)*)column;
    uint32_t flags;
    
    for(uint32_t i = 0; i < entities->components.size; ++i)
    {
        flags = atomicLoad32(&entities->components.data[i].self.flags);
        if(flags & OBJECT_FLAG_REMOVE) entityOnDestroy(&entities->components.data[i]);
    }
}

void entityCompReadyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(Entity)* entities = (ECTColumn(Entity)*)column;
    
    for(uint32_t i = 0; i < entities->components.size; ++i)
    {
        fetchAnd32(&entities->components.data[i].self.flags, ~OBJECT_FLAG_UNREADY);
        entityReady(&entities->components.data[i]);
    }
}

void entityCompDestroyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(Entity)* entities = (ECTColumn(Entity)*)column;
    Entity ent;
    
    for(uint32_t i = 0; i < entities->components.size; ++i)
    {
        entityOnDestroy(&entities->components.data[i]);
    }
    
    while(mwQueuePop(Entity)(&entities->addQueue, &ent))
    {
        entityOnDestroy(&ent);
    }
}

void entityReady(Entity* entity){}
void entityOnDestroy(Entity* entity){} 
