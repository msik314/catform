#ifndef BULLET_SYSTEM_H
#define BULLET_SYSTEM_H

#include <stdint.h>
#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/object.h"
#include "components/bulletComponent.h"
#include "containers/hashmap.h"
#include "containers/collection.h"
#include "cmath/cvec.h"
#include "util/utilMacros.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

void bulletSysInit(ECSystem* self);
void bulletSysDestroy(ECSystem* self);

void bulletCompReady(ECSystem* self, ECTColumn* column);
void bulletSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime);
void bulletCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime);
void bulletCompDestroy(ECSystem* self, ECTColumn* column);

void bulletCompReadyAll(ECSystem* self, ECTColumn* column);
void bulletCompDestroyAll(ECSystem* self, ECTColumn* column);

void bulletReady(BulletComponent* bullet);
void bulletOnDestroy(BulletComponent* bullet); 

extern const ECSystem BULLET_SYSTEM;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //BULLET_SYSTEM_H
