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
#include "systems/aabbSystem.h"
#include "util/atomics.h"
#include "util/linalloc.h"
#include "util/utilMacros.h"

#define PLAYER_GRAVITY -19.6f
#define COS_45 0.760406f

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
    PlayerMoveFlags* moveFlags = linalloc(OFFSETOF(PlayerMoveFlags, updates) + numPlayers * sizeof(PlayerMove));
    uint32_t flagIdx = 0;
    const Input* input = inputGetInstance();
    register Vec2 movement;
    register float xInp;
    register float dTarget;
    register bool jumpInp;
    
    moveFlags->numUpdates = numPlayers;
    
    for(uint32_t i = 0; i < numPlayers; ++i)
    {
        if(atomicLoad32(&players[i].self.flags) & OBJECT_FLAG_DISABLED)
        {
            --moveFlags->numUpdates;
            continue;
        }
        
        movement = players[i].velocity;
        
        xInp = 0;
        jumpInp = false;
        
        if(players[i].controller1 != CAT_INVALID_PLAYER)
        {
            xInp += (players[i].horizontal != CAT_INVALID_AXIS) ? inputGetAxis(input, players[i].controller1, players[i].horizontal): 0.0f;
            jumpInp |= players[i].jumpBtn != CAT_INVALID_BUTTON && inputGetButtonDown(input, players[i].controller1, players[i].jumpBtn);
        }
        
        if(players[i].controller2 != CAT_INVALID_PLAYER)
        {
            xInp += (players[i].horizontal != CAT_INVALID_AXIS) ? inputGetAxis(input, players[i].controller2, players[i].horizontal): 0.0f;
            jumpInp |= players[i].jumpBtn != CAT_INVALID_BUTTON && inputGetButtonDown(input, players[i].controller2, players[i].jumpBtn);
        }
        
        xInp = players[i].moveSpeed * CLAMP(xInp, -1, 1);
        dTarget = movement.x - xInp;
        
        if(players[i].grounded)
        {
            if(ABS(dTarget) <= players[i].groundAccel * deltaTime)
            {
                movement.x = xInp;
            }
            else
            {
                movement.x -= SIGN(dTarget) * players[i].groundAccel * deltaTime;
            }
            
            if(jumpInp)
            {
                movement.y += players[i].jumpSpeed;
            }
        }
        else
        {
            if(ABS(dTarget) <= players[i].airAccel * deltaTime)
            {
                movement.x = xInp;
            }
            else
            {
                movement.x -= SIGN(dTarget) * players[i].airAccel * deltaTime;
            }
        }
        moveFlags->updates[i] = (PlayerMove){movement, players[i].self.parent};
        
        ++flagIdx;
    }
    
    *flags = moveFlags;
}

void playerCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime)
{
    PlayerComponent* players = (PlayerComponent*)column->components.data;
    uint32_t numPlayers = column->components.size;
    const PlayerMoveFlags* playerFlags = (const PlayerMoveFlags*)atomicLoadPtr(&flags[SYSTEM(PlayerComponent)]);
    const AabbFlags* aabbFlags = (const AabbFlags*)atomicLoadPtr(&flags[SYSTEM(AabbComponent)]);
    
    for(uint32_t i = 0; i < numPlayers; ++i)
    {
        players[i].grounded = false;
        for(uint32_t j = 0; j < aabbFlags->numCollisions; ++j)
        {
            if(players[i].self.parent == aabbFlags->collisions[j].entity1 && aabbFlags->collisions[j].normal.y >= COS_45)
            {
                players[i].grounded = true;
                break;
            }
            
            if(players[i].self.parent == aabbFlags->collisions[j].entity2 && -aabbFlags->collisions[j].normal.y >= COS_45)
            {
                players[i].grounded = true;
                break;
            }
        }
        
        players[i].velocity = playerFlags->updates[i].velocity;
        if(players[i].grounded)
        {
            players[i].velocity.y = MAX(players[i].velocity.y, 0.0f);
        }
        else
        {
            players[i].velocity.y += PLAYER_GRAVITY * deltaTime;
        }
    }
}

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
