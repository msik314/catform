#ifndef ENTITY_SYSTEM_H
#define ENTITY_SYSTEM_H

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "components/entity.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

void entitySysInit(ECSystem* self);
void entitySysDestroy(ECSystem* self);

void entityCompReady(ECSystem* self, ECTColumn* column);
void entitySysFlags(ECSystem* self, void** flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime);
void entityCompUpdate(ECSystem* self, ECTColumn* column, const void** flags, uint32_t numFlags, float deltaTime);
void entitycompDestroy(ECSystem* self, ECTColumn* column);

void entityCompReadyAll(ECSystem* self, ECTColumn* column);
void entityCompDestroyAll(ECSystem* self, ECTColumn* column);

void entityReady(Entity* entity);
void entityOnDestroy(Entity* entity); 

extern const ECSystem ENTITY_SYSTEM;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //ENTITY_SYSTEM_H
