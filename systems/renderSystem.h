#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

void renderSysInit(ECSystem* self);
void renderSysDestroy(ECSystem* self);
void renderSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime);

extern const ECSystem RENDER_SYSTEM;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //RENDER_SYSTEM_H
