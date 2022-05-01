#include "util/globalDefs.h"
#include "systems/playerSystem.h"

#include "core/input.h"
#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/sceneManager.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "ecs/phase.h"
#include "systems/systems.h"
#include "systems/entitySystem.h"
#include "util/atomics.h"
#include "util/linalloc.h"
#include "util/utilMacros.h"

const JobDependency PLAYER_READY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(PlayerComponent), PHASE_PARENT)}};
const JobDependency PLAYER_COPY_DEPS = {1, {MAKE_JOB_ID(SYSTEM(PlayerComponent), PHASE_UPDATE)}};

const JobDependency PLAYER_DESTROY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(PlayerComponent), PHASE_MARK)}};

static void playerColCreate(ECTColumn* column){ectColumnCreate(PlayerComponent)((ECTColumn(PlayerComponent)*)column);}

const ECSystem PLAYER_SYSTEM =
{
    playerSysInit,
    playerSysDestroy,
    
    playerColCreate,
    
    playerCompReady, &PLAYER_READY_DEPS,
    playerSysUpdate,
    playerCompCopy, &PLAYER_COPY_DEPS,
    playerCompDestroy, &PLAYER_DESTROY_DEPS,
    
    playerCompReadyAll,
    playerCompDestroyAll
};

void playerSysInit(ECSystem* self){}
void playerSysDestroy(ECSystem* self){}

void playerCompReady(ECSystem* self, ECTColumn* column)
{
    ECTColumn(PlayerComponent)* players = (ECTColumn(PlayerComponent)*)column;
    uint32_t idx = players->components.size;
    uint32_t flags;
    
    while(idx)
    {
        --idx;
        
        flags = atomicLoad32(&players->components.data[idx].self.flags);
        if(!(flags & OBJECT_FLAG_UNREADY))
        {
            return;
        }
        
        fetchAnd32(&players->components.data[idx].self.flags, ~OBJECT_FLAG_UNREADY);
        playerReady(&players->components.data[idx]);
    }
}

void playerSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime)
{
    const PlayerComponent* players = getComponentsConst(columns, PlayerComponent);
    uint32_t numPlayers = getNumComponents(columns, PlayerComponent);
    EntityMoveFlags* moveFlags = linalloc(OFFSETOF(EntityMoveFlags, updates) + numPlayers * sizeof(EntityMove));
    uint32_t flagIdx = 0;
    const Input* input = inputGetInstance();
    register Vec2 movement;
    
    moveFlags->numUpdates = numPlayers;
    
    for(uint32_t i = 0; i < numPlayers; ++i)
    {
        if(atomicLoad32(&players[i].self.flags) & OBJECT_FLAG_DISABLED)
        {
            --moveFlags->numUpdates;
            continue;
        }
        
        movement.x = 0.0f;
        movement.y = 0.0f;
        
        if(players[i].controller1 != CAT_INVALID_PLAYER)
        {
            if(players[i].horizontal != CAT_INVALID_AXIS)
            {
                movement.x += inputGetAxis(input, players[i].controller1, players[i].horizontal);
            }
            
            if(players[i].vertical != CAT_INVALID_AXIS)
            {
                movement.y -= inputGetAxis(input, players[i].controller1, players[i].vertical);
            }
        }
        
        if(players[i].controller2 != CAT_INVALID_PLAYER)
        {
            if(players[i].horizontal != CAT_INVALID_AXIS)
            {
                movement.x += inputGetAxis(input, players[i].controller2, players[i].horizontal);
            }
            
            if(players[i].vertical != CAT_INVALID_AXIS)
            {
                movement.y -= inputGetAxis(input, players[i].controller2, players[i].vertical);
            }
        }
        
        movement.x = players[i].moveSpeed * deltaTime * CLAMP(movement.x, -1, 1);
        movement.y = players[i].moveSpeed * deltaTime * CLAMP(movement.y, -1, 1);
        moveFlags->updates[i] = (EntityMove){movement, players[i].self.parent};
        
        ++flagIdx;
    }
    
    *flags = moveFlags;
}

void playerCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime){}

void playerCompDestroy(ECSystem* self, ECTColumn* column)
{
    ECTColumn(PlayerComponent)* players = (ECTColumn(PlayerComponent)*)column;
    uint32_t flags;
    
    for(uint32_t i = 0; i < players->components.size; ++i)
    {
        flags = atomicLoad32(&players->components.data[i].self.flags);
        if(flags & OBJECT_FLAG_REMOVE) playerOnDestroy(&players->components.data[i]);
    }
}

void playerCompReadyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(PlayerComponent)* players = (ECTColumn(PlayerComponent)*)column;
    
    for(uint32_t i = 0; i < players->components.size; ++i)
    {
        fetchAnd32(&players->components.data[i].self.flags, ~OBJECT_FLAG_UNREADY);
        playerReady(&players->components.data[i]);
    }
}

void playerCompDestroyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(PlayerComponent)* players = (ECTColumn(PlayerComponent)*)column;
    PlayerComponent player;
    
    for(uint32_t i = 0; i < players->components.size; ++i)
    {
        playerOnDestroy(&players->components.data[i]);
    }
    
    while(mwQueuePop(PlayerComponent)(&players->addQueue, &player))
    {
        playerOnDestroy(&player);
    }
}

void playerReady(PlayerComponent* player){}

void playerOnDestroy(PlayerComponent* player){} 
