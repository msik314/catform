#include "util/globalDefs.h"
#include "ecs/sceneManager.h"

#include "core/error.h"
#include "ecs/ectColumn.h"
#include "ecs/ecTable.h"
#include "ecs/ecSystem.h"
#include "containers/vector.h"
#include "components/component.h"
#include "util/atomics.h"
#include "util/sync.h"

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

void sceneManagerCreate(SceneManager* sceneManager, uint32_t numSyncThreads, uint32_t numTotalThreads)
{
    ecTableCreate(&sceneManager->ecTable, NUM_COMPONENT_TYPES);
    sceneManager->sysFlags = NULL;
    
    vectorCreate(ECSystem)(&sceneManager->systems);
    sceneManager->columnSystems = (uint32_t*)CAT_MALLOC(NUM_COMPONENT_TYPES * sizeof(uint32_t));
    
    schedulerCreate(&sceneManager->scheduler);
    
    barrierCreate(&sceneManager->phaseBarrier, numSyncThreads);
    barrierCreate(&sceneManager->frameBarrier, numTotalThreads);
    
    atomicStorePtr(&sceneManager->loadingScene, NULL);
}

void sceneManagerDestroy(SceneManager* sceneManager)
{
    register uint32_t colSys;
    for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
    {
        colSys = sceneManager->columnSystems[i];
        APPLY_ARGS(sceneManager->systems.data[colSys], compDestroyAll, &sceneManager->ecTable.columns[i]);
    };
    
    for(uint32_t i = 0; i < sceneManager->systems.size; ++i)
    {
        APPLY(sceneManager->systems.data[i], sysDestroy);
    }
    
    if(sceneManager->sysFlags) CAT_FREE((void*)sceneManager->sysFlags);
    
    barrierDestroy(&sceneManager->frameBarrier);
    barrierDestroy(&sceneManager->phaseBarrier);
    
    schedulerDestroy(&sceneManager->scheduler);
    
    CAT_FREE(sceneManager->columnSystems);
    vectorDestroy(ECSystem)(&sceneManager->systems);
    
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
    ECTColumnAddRemoveFun arFuns[NUM_COMPONENT_TYPES];
    ECTColumnParentFun parentFuns[NUM_COMPONENT_TYPES];
    JobFun job;
    
    uint32_t numColumns;
    register uint32_t colSys;
    
    for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
    {
        colSys = sceneManager->columnSystems[i];
        sceneManager->systems.data[colSys].colCreate(&sceneManager->ecTable.columns[i]);
    };
    
    sceneManager->sysFlags = (SysFlags*)CAT_MALLOC(sceneManager->systems.size * sizeof(SysFlags));
    
    for(uint32_t i = 0; i < sceneManager->systems.size; ++i)
    {
        APPLY(sceneManager->systems.data[i], sysInit);
    }
    
    //Frame Start
    job.genericFun = (JobGenericFun)barrierWait;
    for(uint32_t i = 0; i < barrierGetMaxThreads(&sceneManager->frameBarrier); ++i)
    {
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(sync), &sceneManager->frameBarrier);
    }
    
    //Phase 1
    for(uint32_t i = 0; i < sceneManager->systems.size; ++i)
    {
        job.updateFun = sceneManager->systems.data[i].sysUpdate;
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(sysUpdate), jobArgEncode(i, i));
    }
    
    job.genericFun = (JobGenericFun)barrierWait;
    for(uint32_t i = 0; i < barrierGetMaxThreads(&sceneManager->phaseBarrier); ++i)
    {
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(sync), &sceneManager->phaseBarrier);
    }
    
    //Phase 2
    for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
    {
        colSys = sceneManager->columnSystems[i];
        job.copyFun = sceneManager->systems.data[colSys].compCopy;
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(compCopy), jobArgEncode(colSys, i));
    }
    
    job.genericFun = (JobGenericFun)barrierWait;
    for(uint32_t i = 0; i < barrierGetMaxThreads(&sceneManager->phaseBarrier); ++i)
    {
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(sync), &sceneManager->phaseBarrier);
    }
    
    //Phase 3
    numColumns = ecTableGetMarkFuns(&sceneManager->ecTable, parentFuns, NUM_COMPONENT_TYPES);
    for(uint32_t i = 0; i < numColumns; ++i)
    {
        job.parentFun = parentFuns[i];
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(colMark), jobArgEncode(0xffffffff, i));
    }
    
    job.genericFun = (JobGenericFun)barrierWait;
    for(uint32_t i = 0; i < barrierGetMaxThreads(&sceneManager->phaseBarrier); ++i)
    {
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(sync), &sceneManager->phaseBarrier);
    }
    
    //Phase 4
    for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
    {
        colSys = sceneManager->columnSystems[i];
        job.destroyFun = sceneManager->systems.data[colSys].compDestroy;
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(compDestroy), jobArgEncode(colSys, i));
    }
    
    job.genericFun = (JobGenericFun)barrierWait;
    for(uint32_t i = 0; i < barrierGetMaxThreads(&sceneManager->phaseBarrier); ++i)
    {
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(sync), &sceneManager->phaseBarrier);
    }
    
    //Phase 5
    numColumns = ecTableGetARFuns(&sceneManager->ecTable, arFuns, NUM_COMPONENT_TYPES);
    for(uint32_t i = 0; i < numColumns; ++i)
    {
        job.addRemoveFun = arFuns[i];
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(colAddRemove), jobArgEncode(0xffffffff, i));
    }
    
    job.genericFun = (JobGenericFun)barrierWait;
    for(uint32_t i = 0; i < barrierGetMaxThreads(&sceneManager->phaseBarrier); ++i)
    {
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(sync), &sceneManager->phaseBarrier);
    }
    
    //Phase 6
    numColumns = ecTableGetParentFuns(&sceneManager->ecTable, parentFuns, NUM_COMPONENT_TYPES);
    for(uint32_t i = 0; i < numColumns; ++i)
    {
        job.parentFun = parentFuns[i];
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(colParent), jobArgEncode(0xffffffff, i));
    }
    
    job.genericFun = (JobGenericFun)barrierWait;
    for(uint32_t i = 0; i < barrierGetMaxThreads(&sceneManager->phaseBarrier); ++i)
    {
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(sync), &sceneManager->phaseBarrier);
    }
    
    //Phase 7
    for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
    {
        colSys = sceneManager->columnSystems[i];
        job.readyFun = sceneManager->systems.data[colSys].compReady;
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(compReady), jobArgEncode(colSys, i));
    }
    
    job.genericFun = (JobGenericFun)barrierWait;
    for(uint32_t i = 0; i < barrierGetMaxThreads(&sceneManager->phaseBarrier); ++i)
    {
        schedulerRegister(&sceneManager->scheduler, job, JOB_TYPE(sync), &sceneManager->phaseBarrier);
    }
}

bool sceneManagerSwitchScene(SceneManager* sceneManager, ECTable* table)
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

bool sceneManagerLoadScene(SceneManager* sceneManager, const JsonData* scene)
{
    ECTable* table;
    uint32_t colSys;
    uint32_t tableIdx;
    
    table = (ECTable*)CAT_MALLOC(sizeof(ECTable));
    memset(table, 0, sizeof(ECTable));
    ecTableCreate(table, NUM_COMPONENT_TYPES);
    
    for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
    {
        colSys = sceneManager->columnSystems[i];
        sceneManager->systems.data[colSys].colCreate(&table->columns[i]);
    }
    
    jsonObjectGetKey(&scene->root, "table", &tableIdx);
    ecTableDeserialize(table, scene, tableIdx);
    ecTableAddRemove(table);
    
    return sceneManagerSwitchScene(sceneManager, table);
}

static inline void runSchedule(Scheduler* scheduler, ECTable* table, ECSystem* systems, SysFlags* sysFlags, uint32_t numSystems, float deltaTime)
{
    Job job;
    uint32_t sysIdx;
    uint32_t colIdx;
    
    while(schedulerGetNext(scheduler, &job))
    {
        switch(job.jobType)
        {
        case JOB_TYPE(compReady):
            jobArgDecode(job.args, &sysIdx, &colIdx);
            job.function.readyFun(&systems[sysIdx], &table->columns[colIdx]);
            break;
        
        case JOB_TYPE(sysUpdate):
            jobArgDecode(job.args, &sysIdx, &colIdx);
            job.function.updateFun(&systems[sysIdx], &sysFlags[sysIdx], (const ECTColumn*)table->columns, table->numColumns, deltaTime);
            break;
        
        case JOB_TYPE(compCopy):
            jobArgDecode(job.args, &sysIdx, &colIdx);
            job.function.copyFun(&systems[sysIdx], &table->columns[colIdx], (const SysFlags*)sysFlags, numSystems, deltaTime);
            break;
        
        case JOB_TYPE(compDestroy):
            jobArgDecode(job.args, &sysIdx, &colIdx);
            job.function.destroyFun(&systems[sysIdx], &table->columns[colIdx]);
            break;
        
        case JOB_TYPE(colMark):
        case JOB_TYPE(colParent):
            jobArgDecode(job.args, &sysIdx, &colIdx);
            job.function.parentFun(&table->columns[colIdx], &table->columns[COMPONENT(Entity)], &table->pointerMap);
            break;
        
        case JOB_TYPE(colAddRemove):
            jobArgDecode(job.args, &sysIdx, &colIdx);
            job.function.addRemoveFun(&table->columns[colIdx], &table->pointerMap);
            break;
        
        case JOB_TYPE(sync):
            job.function.genericFun(job.args); //Technically undefined behavior, void (*)(void*) != void (*)(Barrier*)
            break;
        
        default:
            printWarn(CAT_WARNING_EC_SYSTEM, "invalid job type encountered. Skipping...");
            break;
        }
    }
}

void sceneManagerFrame(SceneManager* sceneManager, float deltaTime, bool lastFrame)
{
    ECTable* newTable;
    register uint32_t colSys;

    runSchedule(&sceneManager->scheduler, &sceneManager->ecTable, sceneManager->systems.data, sceneManager->sysFlags, sceneManager->systems.size, deltaTime);
    
    if(!lastFrame)schedulerReset(&sceneManager->scheduler);
    
    newTable = (ECTable*)atomicStorePtr(&sceneManager->loadingScene, NULL);
    if(newTable)
    {
        //Destroy all
        for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
        {
            colSys = sceneManager->columnSystems[i];
            APPLY_ARGS(sceneManager->systems.data[colSys], compDestroyAll, &sceneManager->ecTable.columns[i]);
        };
        
        //Remove all
        ecTableRemoveAll(&sceneManager->ecTable);
        
        //Replace table
        ecTableDestroy(&sceneManager->ecTable);
        sceneManager->ecTable = *newTable;
        CAT_FREE(newTable);
        
        //Ready all
        for(uint32_t i = 0; i < sceneManager->ecTable.numColumns; ++i)
        {
            colSys = sceneManager->columnSystems[i];
            APPLY_ARGS(sceneManager->systems.data[colSys], compReadyAll, &sceneManager->ecTable.columns[i]);
        };
    }
}

void sceneManagerFollowFrame(SceneManager* sceneManager, float deltaTime)
{
    runSchedule(&sceneManager->scheduler, &sceneManager->ecTable, sceneManager->systems.data, sceneManager->sysFlags, sceneManager->systems.size, deltaTime);
}
