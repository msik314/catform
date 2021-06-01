#include "util/globalDefs.h"
#include "systems/entitySystem.h"

#include <string.h>
#include <stdio.h>

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/sceneManager.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "core/tag.h"
#include "components/entity.h"
#include "util/atomics.h"

static void entityColCreate(ECTColumn* column){ectColumnCreate(Entity)((ECTColumn(Entity)*)column);}

const ECSystem ENTITY_SYSTEM =
{
    entitySysInit,
    entitySysDestroy,
    
    entityColCreate,
    
    entityCompReady,
    entitySysFlags,
    entityCompUpdate,
    entitycompDestroy,
    
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

void entitySysFlags(ECSystem* self, void** flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime){}

static inline uint32_t markIfParentMarked(Entity* entities, bool* visited, uint32_t idx, const PointerMap* map)
{
    uint32_t parentIdx;
    uint32_t flags;
    
    if(visited[idx]) return atomicLoad32(&entities[idx].self.flags) & OBJECT_FLAG_REMOVE;
    visited[idx] = true;
    
    if(entities[idx].self.parent == INVALID_OBJECT) return atomicLoad32(&entities[idx].self.flags) & OBJECT_FLAG_REMOVE;
    
    parentIdx = pointerMapGet(map, entities[idx].self.parent);
    flags = markIfParentMarked(entities, visited, parentIdx, map);
    return (fetchOr32(&entities[idx].self.flags, flags) | flags) & OBJECT_FLAG_REMOVE; 
}

void entityCompUpdate(ECSystem* self, ECTColumn* column, const void** flags, uint32_t numFlags, float deltaTime)
{
    ECTColumn(Entity)* entities = (ECTColumn(Entity)*)column;
    bool entityVisited[entities->components.size];
    uint32_t* testCompFlags = (uint32_t*)flags[1]; //TestComp system
    const PointerMap* map = sceneManagerGetMap(sceneManagerGetInstance());
    Entity* entity;
    
    
    for(uint32_t i = 0; i < testCompFlags[0]; ++i)
    {
        entity = &entities->components.data[testCompFlags[2 * i + 2]];
        fetchOr32(&entity->self.flags, (testCompFlags[2 * i + 1] >= 16) * OBJECT_FLAG_REMOVE);
    }
    
    memset(entityVisited, 0, entities->components.size * sizeof(bool));
    
    for(uint32_t i = 0; i < entities->components.size; ++i)
    {
        markIfParentMarked(entities->components.data, entityVisited, i, map);
    }
}

void entitycompDestroy(ECSystem* self, ECTColumn* column)
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
void entityOnDestroy(Entity* entity)
{
    char buffer[sizeof(Tag) + 1] = {};
    strncpy(buffer, entity->name.data, sizeof(Tag));
    printf("Destroying entity %s\n", buffer);
} 
