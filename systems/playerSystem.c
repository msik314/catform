#include "util/globalDefs.h"
#include "systems/playerSystem.h"

#include <math.h>
#include "cmath/cVec.h"
#include "core/input.h"
#include "core/tag.h"
#include "ecs/ecSystem.h"
#include "ecs/ecTable.h"
#include "ecs/ectColumn.h"
#include "ecs/sceneManager.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "ecs/phase.h"
#include "ecs/tableCache.h"
#include "systems/systems.h"
#include "systems/aabbSystem.h"
#include "util/atomics.h"
#include "util/linalloc.h"
#include "util/utilMacros.h"
#include "util/resourceMap.h"
#include "components/entity.h"
#include "components/aabbComponent.h"
#include "components/bulletComponent.h"
#include "components/spriteComponent.h"
#include "json/jsonReader.h"
#include "json/jsonData.h"

#define PLAYER_GRAVITY -39.2f
#define COS_45 0.7071067811f
#define BULLET_SPAWN_OFFSET 3
#define BULLET_GRAVITY -39.2f
#define BULLET_SPEED 64.0f
#define BULLET_FALL_DELAY 0.5f
#define BULLET_LIFETIME 2.0f
#define BULLET_DAMAGE 0.0f
#define BULLET_KNOCKBACK 0.0f

const JobDependency PLAYER_READY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(PlayerComponent), PHASE_PARENT)}};
const JobDependency PLAYER_COPY_DEPS = {2, {MAKE_JOB_ID(SYSTEM(PlayerComponent), PHASE_UPDATE), MAKE_JOB_ID(SYSTEM(AabbComponent), PHASE_UPDATE)}};

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

static inline void spawnBullet(Vec2 position, Vec2 direction, float lastDirection, uint32_t playerNum)
{
    ECTable* table = sceneManagerGetTable(sceneManagerGetInstance());
    ECTable* prefab;
    JsonData data;
    Entity* entities;
    BulletComponent* bullets;
    AabbComponent* aabbs;
    Tag pfbName;
    uint32_t tableIdx;
    register float dirLen;
    
    if(ABS(direction.x) < 0.5 && ABS(direction.y) < 0.5)
    {
        if(ABS(lastDirection) < 0.5) return;
        
        direction.x = lastDirection;
    }
    
    tagSet(&pfbName, "bulletPfb");
    prefab = tableCacheGet(tableCacheGetInstance(), pfbName);
    
    entities = getComponents(prefab->columns, Entity);
    entities[0].transform.position.x = position.x;
    entities[0].transform.position.y = position.y;
    
    direction.x = (direction.x > 0.5f) - (direction.x < -0.5f);
    direction.y = (direction.y > 0.5f) - (direction.y < -0.5f);
    dirLen = sqrtf(direction.x * direction.x + direction.y * direction.y);
    
    direction.x /= dirLen;
    direction.y /= dirLen;
    
    entities[0].transform.position.x += BULLET_SPAWN_OFFSET * direction.x;
    entities[0].transform.position.y += BULLET_SPAWN_OFFSET * direction.y;
    
    bullets = getComponents(prefab->columns, BulletComponent);
    bullets[0].velocity.x = BULLET_SPEED * direction.x;
    bullets[0].velocity.y = BULLET_SPEED * direction.y;
    
    aabbs = getComponents(prefab->columns, AabbComponent);
    aabbs[0].layerMask = 16 << playerNum;
    aabbs[0].collisionMask = ~(1 << playerNum);
    
    ecTableAddPrefabTable(table, prefab);
}

void playerSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime)
{
    const PlayerComponent* players = getComponentsConst(columns, PlayerComponent);
    uint32_t numPlayers = getNumComponents(columns, PlayerComponent);
    PlayerMoveFlags* moveFlags = linalloc(OFFSETOF(PlayerMoveFlags, updates) + numPlayers * sizeof(PlayerMove));
    const Entity* entities = getComponentsConst(columns, Entity);
    const PointerMap* map = sceneManagerGetMap(sceneManagerGetInstance());
    uint32_t flagIdx = 0;
    const Input* input = inputGetInstance();
    register Vec2 movement;
    register float xInp;
    register float yInp;
    register float dTarget;
    register uint32_t idx;
    register bool jumpInp;
    register bool shootInp;
    register float lastDirection;
    
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
        yInp = 0;
        jumpInp = false;
        shootInp = false;
        
        if(players[i].controller1 != CAT_INVALID_PLAYER)
        {
            xInp += (players[i].horizontal != CAT_INVALID_AXIS) ? inputGetAxis(input, players[i].controller1, players[i].horizontal): 0.0f;
            yInp -= (players[i].vertical != CAT_INVALID_AXIS) ? inputGetAxis(input, players[i].controller1, players[i].vertical): 0.0f;
            jumpInp |= players[i].jumpBtn != CAT_INVALID_BUTTON && inputGetButtonDown(input, players[i].controller1, players[i].jumpBtn);
            shootInp |= players[i].shootBtn != CAT_INVALID_BUTTON && inputGetButtonDown(input, players[i].controller1, players[i].shootBtn);
        }
        
        if(players[i].controller2 != CAT_INVALID_PLAYER)
        {
            xInp += (players[i].horizontal != CAT_INVALID_AXIS) ? inputGetAxis(input, players[i].controller2, players[i].horizontal): 0.0f;
            yInp -= (players[i].vertical != CAT_INVALID_AXIS) ? inputGetAxis(input, players[i].controller2, players[i].vertical): 0.0f;
            jumpInp |= players[i].jumpBtn != CAT_INVALID_BUTTON && inputGetButtonDown(input, players[i].controller2, players[i].jumpBtn);
            shootInp |= players[i].shootBtn != CAT_INVALID_BUTTON && inputGetButtonDown(input, players[i].controller2, players[i].shootBtn);
        }
        
        xInp = CLAMP(xInp, -1, 1);
        lastDirection = xInp;
        
        yInp = CLAMP(yInp, -1, 1);
        
        if(shootInp)
        {
            idx = pointerMapGet(map, players[i].self.parent);
            spawnBullet
            (
                (Vec2){entities[idx].transform.position.x, entities[idx].transform.position.y},
                (Vec2){xInp, yInp},
                players[i].lastDirection,
                players[i].playerNum
            );
        }
        
        xInp *= players[i].moveSpeed;
        dTarget = movement.x - xInp;
        
        if(players[i].colliding & PLAYER_DIRECTION_DOWN)
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
        
        moveFlags->updates[flagIdx] = (PlayerMove){movement, players[i].self.parent, lastDirection};
        
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
    uint32_t flagIdx = 0;
    
    register Vec2 normal;
    
    for(uint32_t i = 0; i < numPlayers; ++i)
    {
        if(atomicLoad32(&players[i].self.flags) & OBJECT_FLAG_DISABLED)
        {
            continue;
        }
        
        players[i].colliding = 0;
        players[i].velocity = playerFlags->updates[flagIdx].velocity;
        if(ABS(playerFlags->updates[flagIdx].input) > 0.5f)
        {
            players[i].lastDirection = playerFlags->updates[flagIdx].input;
        }
        
        for(uint32_t j = 0; j < aabbFlags->numCollisions; ++j)
        {
            normal = (Vec2){};
            if(players[i].self.parent == aabbFlags->collisions[j].entity1)
            {
                normal = aabbFlags->collisions[j].normal;
            }
            
            else if(players[i].self.parent == aabbFlags->collisions[j].entity2)
            {
                normal =  aabbFlags->collisions[j].normal;
                normal.x *= -1;
                normal.y *= -1;
            }
            
            if(normal.y >= COS_45)
            {
                players[i].colliding |= PLAYER_DIRECTION_DOWN;
            }
            else if(-normal.y >= COS_45)
            {
                players[i].colliding |= PLAYER_DIRECTION_UP;
            }
            else if(normal.x >= COS_45)
            {
                players[i].colliding |= PLAYER_DIRECTION_LEFT;
                players[i].velocity.x = MAX(players[i].velocity.x, 0.0f);
            }
            else if(-normal.x >= COS_45)
            {
                players[i].colliding |= PLAYER_DIRECTION_RIGHT;
                players[i].velocity.x = MIN(players[i].velocity.x, 0.0f);
            }
        }
        
        if(players[i].colliding & PLAYER_DIRECTION_DOWN)
        {
            players[i].velocity.y = MAX(players[i].velocity.y, 0.0f);
        }
        else
        {
            if(players[i].colliding & PLAYER_DIRECTION_UP)
            {
                players[i].velocity.y = MIN(players[i].velocity.y, 0.0f);
            }
            players[i].velocity.y += PLAYER_GRAVITY * deltaTime;
        }
        
        ++flagIdx;
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

void playerReady(PlayerComponent* player)
{
    Tag t;
    tagSet(&t, "bulletPfb");
    resourceMapLoadTable(resourceMapGetInstance(), t); //This has to be done during ready phase
}

void playerOnDestroy(PlayerComponent* player){}
