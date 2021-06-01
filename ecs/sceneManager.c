#include "util/globalDefs.h"
#include "ecs/sceneManager.h"

#include "ecs/ecTable.h"
#include "ecs/ecSystem.h"
#include "containers/vector.h"
#include "components/component.h"
#include "util/atomics.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

#define APPLY(SYSTEM, FUNCTION)\
if(SYSTEM.FUNCTION) SYSTEM.FUNCTION(&SYSTEM)

#define APPLY_ARGS(SYSTEM, FUNCTION, ...)\
if(SYSTEM.FUNCTION) SYSTEM.FUNCTION(&SYSTEM, __VA_ARGS__)


SceneManager* sceneManagerGetInstance()
{
    static SceneManager instance;
    return &instance;
}

void sceneManagerCreate(SceneManager* sceneManager)
{
    ecTableCreate(&sceneManager->ecTable, NUM_COMPONENT_TYPES);
    sceneManager->columnSystems = (uint32_t*)CAT_MALLOC(NUM_COMPONENT_TYPES * sizeof(uint32_t));
    
    vectorCreate(ECSystem)(&sceneManager->systems);
    sceneManager->sysFlags = NULL;
    atomicStorePtr(&sceneManager->loadingScene, NULL);
}

void sceneManagerDestroy(SceneManager* sceneManager)
{
    uint32_t colSys;
    for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
    {
        colSys = sceneManager->columnSystems[i];
        APPLY_ARGS(sceneManager->systems.data[colSys], compDestroyAll, &sceneManager->ecTable.columns[i]);
    };
    
    for(uint32_t i = 0; i < sceneManager->systems.size; ++i)
    {
        APPLY(sceneManager->systems.data[i], sysDestroy);
    }
    
    if(sceneManager->sysFlags) CAT_FREE(sceneManager->sysFlags);
    vectorDestroy(ECSystem)(&sceneManager->systems);
    
    CAT_FREE(sceneManager->columnSystems);
    
    ecTableDestroy(&sceneManager->ecTable);
}

void sceneManagerRegisterSystem(SceneManager* sceneManager, const ECSystem* system)
{
    vectorAdd(ECSystem)(&sceneManager->systems, system);
}

void sceneManagerRegisterColumnSys(SceneManager* sceneManager, const ECSystem* system, uint32_t column)
{
    sceneManager->columnSystems[column] = sceneManager->systems.size;
    vectorAdd(ECSystem)(&sceneManager->systems, system);
}

void sceneManagerInit(SceneManager* sceneManager)
{
    uint32_t colSys;
    for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
    {
        colSys = sceneManager->columnSystems[i];
        sceneManager->systems.data[colSys].colCreate(&sceneManager->ecTable.columns[i]);
    };
    
    sceneManager->sysFlags = (void**)CAT_MALLOC(sceneManager->systems.size * sizeof(void*));
    
    for(uint32_t i = 0; i < sceneManager->systems.size; ++i)
    {
        APPLY(sceneManager->systems.data[i], sysInit);
    }
}

bool sceneManagerLoadScene(SceneManager* sceneManager, ECTable* table)
{
    ECTable* expected = NULL;
    ECTable* newTable = (ECTable*)1;
    Object* obj;
    
    while(1)
    {
        if(compareAndSwapPtr(&sceneManager->loadingScene, &expected, newTable)) break;
        
        if(expected) return false;
    }
    
    newTable = CAT_MALLOC(sizeof(ECTable));
    *newTable = *table;
    atomicStorePtr(&sceneManager->loadingScene, newTable);
    
    return true;
}

void sceneManagerFrame(SceneManager* sceneManager, float deltaTime)
{
    ECTable* newTable;
    register uint32_t colSys;
    
    //Phase 1
    for(uint32_t i = 0; i < sceneManager->systems.size; ++i)
    {
        APPLY_ARGS(sceneManager->systems.data[i], sysFlags, &sceneManager->sysFlags[i], sceneManager->ecTable.columns, sceneManager->ecTable.numColumns, deltaTime);
    }
    
    //Phase 2
    for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
    {
        colSys = sceneManager->columnSystems[i];
        APPLY_ARGS(sceneManager->systems.data[colSys], compUpdate, &sceneManager->ecTable.columns[i], (const void**)sceneManager->sysFlags, sceneManager->systems.size, deltaTime);
    }
    
    newTable = (ECTable*)atomicLoadPtr(&sceneManager->loadingScene);
    if(newTable)
    {
        //Phase 3
            //Pass
        
        //Phase 4
        for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
        {
            colSys = sceneManager->columnSystems[i];
            APPLY_ARGS(sceneManager->systems.data[colSys], compDestroyAll, &sceneManager->ecTable.columns[i]);
        };
        
        //Phase 5
        ecTableRemoveAll(&sceneManager->ecTable);
        
        //Synchronous between phases 5 and 6
        ecTableDestroy(&sceneManager->ecTable);
        sceneManager->ecTable = *newTable;
        CAT_FREE(newTable);
        
        atomicStorePtr(&sceneManager->loadingScene, NULL);
        
        //Phase 6
        for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
        {
            colSys = sceneManager->columnSystems[i];
            APPLY_ARGS(sceneManager->systems.data[colSys], compReadyAll, &sceneManager->ecTable.columns[i]);
        };
        
    }
    else
    {
        //Phase 3
        ecTableMark(&sceneManager->ecTable);
        
        //Phase 4
        for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
        {
            colSys = sceneManager->columnSystems[i];
            APPLY_ARGS(sceneManager->systems.data[colSys], compDestroy, &sceneManager->ecTable.columns[i]);
        };
    
        //Phase 5
        ecTableAddRemove(&sceneManager->ecTable);
        
        //Phase 6
        for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
        {
            colSys = sceneManager->columnSystems[i];
            APPLY_ARGS(sceneManager->systems.data[colSys], compReady, &sceneManager->ecTable.columns[i]);
        };
    }
}
