#include "util/globalDefs.h"
#include "ecs/scheduler.h"

#include "containers/vector.h"
#include "util/atomics.h"
#include "util/sync.h"
#include "util/utilMacros.h"

#ifndef CAT_MALLOC
#include <stdlib.h>
#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

void schedulerCreate(Scheduler* scheduler, uint32_t numJobs)
{
    vectorCreate(Job)(&scheduler->queue);
    atomicStore32(&scheduler->nextJob, 0);
    
    scheduler->jobCounters = CAT_MALLOC(numJobs * sizeof(uint32_t));
    for(uint32_t i = 0; i < numJobs; ++i)
    {
        atomicStore32(&scheduler->jobCounters[i], 0);
    }
    
    scheduler->numJobCounters = numJobs;
    atomicStore32(&scheduler->frameCounter, 0); //Rolls over to 0 on first reset
}

void schedulerDestroy(Scheduler* scheduler)
{
    CAT_FREE((void*)scheduler->jobCounters);
    vectorDestroy(Job)(&scheduler->queue);
}

void schedulerRegister(Scheduler* scheduler, JobFun function, JobType jobType, void* args, uint32_t id)
{
    Job job = {function, args, jobType, id};
    vectorAdd(Job)(&scheduler->queue, &job);
}

void schedulerReset(Scheduler* scheduler)
{
    fetchAdd32(&scheduler->frameCounter, 1);
    atomicStore32(&scheduler->nextJob, 0);
}


bool schedulerGetNext(Scheduler* scheduler, Job* outJob)
{
    uint32_t idx = fetchAdd32(&scheduler->nextJob, 1);
    if(idx >= scheduler->queue.size) return false;
    
    *outJob = scheduler->queue.data[idx];
    return true;
}

void schedulerFinish(Scheduler* scheduler, uint32_t jobId)
{
    if(jobId < scheduler->numJobCounters)
    {
        fetchAdd32(&scheduler->jobCounters[jobId], 1);
    }
}

void schedulerWaitDeps(Scheduler* scheduler, const uint32_t* ids, uint32_t numIDs)
{
    register uint32_t id;
    
    for(uint32_t i = 0; i < numIDs; ++i)
    {
        id = ids[i];
        while(atomicLoad32(&scheduler->jobCounters[id]) != atomicLoad32(&scheduler->frameCounter))
        {
            SPIN();
        }
    }
}
