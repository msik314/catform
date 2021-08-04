#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "ecs/ecTable.h"
#include "ecs/ecSystem.h"
#include "ecs/pointerMap.h"
#include "ecs/scheduler.h"
#include "containers/vector.h"
#include "util/sync.h"
#include "json/jsonData.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    ECTable ecTable;
    SysFlags* sysFlags;
    
    Vector(ECSystem) systems;
    uint32_t* columnSystems;
    
    Scheduler scheduler;
    Barrier frameBarrier;
    
    ECTable volatile* loadingScene;
}
SceneManager;

SceneManager* sceneManagerGetInstance();

void sceneManagerCreate(SceneManager* sceneManager, uint32_t numThreads);
void sceneManagerDestroy(SceneManager* sceneManager);

static inline const PointerMap* sceneManagerGetMap(SceneManager* sceneManager) {return &sceneManager->ecTable.pointerMap;}
static inline ECTable* sceneManagerGetTable(SceneManager* sceneManager) {return &sceneManager->ecTable;}

void sceneManagerRegisterSystem(SceneManager* sceneManager, const ECSystem* system);
void sceneManagerRegisterColumnSys(SceneManager* sceneManager, const ECSystem* system, uint32_t column);
void sceneManagerInit(SceneManager* sceneManager);

bool sceneManagerSwitchScene(SceneManager* sceneManager, ECTable* table);
bool sceneManagerLoadScene(SceneManager* sceneManager, const JsonData* scene);
void sceneManagerFrame(SceneManager* sceneManager, float deltaTime);

void sceneManagerFollowFrame(SceneManager* sceneManager, float deltaTime);

static inline void sceneManagerWaitFrame(SceneManager* sceneManager) {barrierWait(&sceneManager->frameBarrier);}

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SCENE_MANAGER_H
