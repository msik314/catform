#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "ecs/ectColumn.h"
#include "ecs/ecSystem.h"
#include "containers/vector.h"
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
    JobType jobType;
    void* args;
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
}
Scheduler;

void schedulerCreate(Scheduler* scheduler);
void schedulerDestroy(Scheduler* scheduler);

void schedulerRegister(Scheduler* scheduler, JobFun function, JobType jobType, void* args);

void schedulerReset(Scheduler* scheduler);
bool schedulerGetNext(Scheduler* scheduler, Job* outJob);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SCHEDULER_H
