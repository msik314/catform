#ifndef PLAYER_SYSTEM_H
#define PLAYER_SYSTEM_H

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "components/playerComponent.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    Vec2 velocity;
    ObjectID parent;
    float input;
}
PlayerMove;

typedef struct
{
    uint32_t numUpdates;
    PlayerMove updates[];
}
PlayerMoveFlags;

void playerSysInit(ECSystem* self);
void playerSysDestroy(ECSystem* self);

void playerCompReady(ECSystem* self, ECTColumn* column);
void playerSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime);
void playerCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime);
void playerCompDestroy(ECSystem* self, ECTColumn* column);

void playerCompReadyAll(ECSystem* self, ECTColumn* column);
void playerCompDestroyAll(ECSystem* self, ECTColumn* column);

void playerReady(PlayerComponent* player);
void playerOnDestroy(PlayerComponent* player); 

extern const ECSystem PLAYER_SYSTEM;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //PLAYER_SYSTEM_H
