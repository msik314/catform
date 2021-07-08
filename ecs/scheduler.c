#include "util/globalDefs.h"
#include "ecs/scheduler.h"

#include "containers/vector.h"
#include "util/atomics.h"
#include "util/sync.h"
#include "util/utilMacros.h"

void schedulerCreate(Scheduler* scheduler)
{
    vectorCreate(Job)(&scheduler->queue);
    atomicStore32(&scheduler->nextJob, 0);
}

void schedulerDestroy(Scheduler* scheduler)
{
    vectorDestroy(Job)(&scheduler->queue);
}

void schedulerRegister(Scheduler* scheduler, JobFun function, JobType jobType, void* args)
{
    Job job = {function, jobType, args};
    vectorAdd(Job)(&scheduler->queue, &job);
}

void schedulerReset(Scheduler* scheduler)
{
    atomicStore32(&scheduler->nextJob, 0);
}

bool schedulerGetNext(Scheduler* scheduler, Job* outJob)
{
    uint32_t idx = fetchAdd32(&scheduler->nextJob, 1);
    if(idx >= scheduler->queue.size) return false;
    
    *outJob = scheduler->queue.data[idx];
    return true;
}
