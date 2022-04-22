#ifndef SPRITE_SYSTEM_H
#define SPRITE_SYSTEM_H

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "components/spriteComponent.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

void spriteSysInit(ECSystem* self);
void spriteSysDestroy(ECSystem* self);

void spriteCompReady(ECSystem* self, ECTColumn* column);
void spriteSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime);
void spriteCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime);
void spriteCompDestroy(ECSystem* self, ECTColumn* column);

void spriteCompReadyAll(ECSystem* self, ECTColumn* column);
void spriteCompDestroyAll(ECSystem* self, ECTColumn* column);

void spriteReady(SpriteComponent* sprite);
void spriteOnDestroy(SpriteComponent* sprite); 

extern const ECSystem SPRITE_SYSTEM;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SPRITE_SYSTEM_H
