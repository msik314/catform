#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "ecs/ectColumn.h"
#include "ecs/ecSystem.h"
#include "containers/vector.h"
#include "systems/systems.h"
#include "util/utilMacros.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define JOB_TYPE(TYPE) JOB_TYPE_ ## TYPE

typedef enum
{
    JOB_TYPE(compReady),
    JOB_TYPE(sysUpdate),
    JOB_TYPE(compCopy),
    JOB_TYPE(compDestroy),
    
    JOB_TYPE(colMark),
    JOB_TYPE(colAddRemove),
    JOB_TYPE(colParent),
    
    JOB_TYPE(sync)
}
JobType;

typedef void(*JobGenericFun)(void*);

typedef union
{
    JobGenericFun genericFun;

    SysReadyFun readyFun;
    SysUpdateFun updateFun;
    SysCopyFun copyFun;
    SysComponentDestroyFun destroyFun;
    
    ECTColumnParentFun parentFun;
    ECTColumnAddRemoveFun addRemoveFun;
}
JobFun;

typedef struct
{
    JobFun function;
    void* args;
    JobType jobType;
    uint32_t id;
}
Job;
VECTOR_DECL(Job);

static inline void* jobArgEncode(uint32_t sysIdx, uint32_t colIdx) {return (void*)(((uint64_t)sysIdx << 32) | colIdx);}
static inline void jobArgDecode(void* args, uint32_t* outSysIdx, uint32_t* outColIdx)
{
    register uint64_t argsInt = (uint64_t)args;
    *outColIdx = (uint32_t)(argsInt & 0x00000000ffffffff);
    *outSysIdx = (uint32_t)((argsInt >> 32) & 0x00000000ffffffff);
}

typedef struct
{
    Vector(Job) queue;
    volatile uint32_t nextJob;
    
    volatile uint32_t* jobCounters;
    uint32_t numJobCounters;
    volatile uint32_t frameCounter;
}
Scheduler;

void schedulerCreate(Scheduler* scheduler, uint32_t numJobs);
void schedulerDestroy(Scheduler* scheduler);

void schedulerRegister(Scheduler* scheduler, JobFun function, JobType jobType, void* args, uint32_t id);

void schedulerReset(Scheduler* scheduler);
bool schedulerGetNext(Scheduler* scheduler, Job* outJob);
void schedulerFinish(Scheduler* scheduler, uint32_t jobId);

void schedulerWaitDeps(Scheduler* scheduler, const uint32_t* ids, uint32_t numIDs);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SCHEDULER_H
