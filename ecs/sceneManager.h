#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "ecs/ecTable.h"
#include "ecs/ecSystem.h"
#include "ecs/pointerMap.h"
#include "containers/vector.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    ECTable ecTable;
    
    Vector(ECSystem) systems;
    void** sysFlags;
    uint32_t* columnSystems;
    
    volatile ECTable* loadingScene;
}
SceneManager;

SceneManager* sceneManagerGetInstance();

void sceneManagerCreate(SceneManager* sceneManager);
void sceneManagerDestroy(SceneManager* sceneManager);

static inline const PointerMap* sceneManagerGetMap(SceneManager* sceneManager){return &sceneManager->ecTable.pointerMap;}
static inline ECTable* sceneManagerGetTable(SceneManager* sceneManager){return &sceneManager->ecTable;}

void sceneManagerRegisterSystem(SceneManager* sceneManager, const ECSystem* system);
void sceneManagerRegisterColumnSys(SceneManager* sceneManager, const ECSystem* system, uint32_t column);
void sceneManagerInit(SceneManager* sceneManager);

bool sceneManagerLoadScene(SceneManager* sceneManager, ECTable* table);
void sceneManagerFrame(SceneManager* sceneManager, float deltaTime);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SCENE_MANAGER_H
