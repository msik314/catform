#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "components/cameraComponent.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

void cameraSysInit(ECSystem* self);
void cameraSysDestroy(ECSystem* self);

void cameraCompReady(ECSystem* self, ECTColumn* column);
void cameraSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime);
void cameraCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime);
void cameraCompDestroy(ECSystem* self, ECTColumn* column);

void cameraCompReadyAll(ECSystem* self, ECTColumn* column);
void cameraCompDestroyAll(ECSystem* self, ECTColumn* column);

void cameraReady(CameraComponent* camera);
void cameraOnDestroy(CameraComponent* camera); 

extern const ECSystem CAMERA_SYSTEM;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //CAMERA_SYSTEM_H
