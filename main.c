#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "core/tag.h"
#include "components/entity.h"
#include "components/testComp.h"
#include "ecs/ectColumn.h"
#include "ecs/pointerMap.h"
#include "ecs/ecTable.h"
#include "ecs/sceneManager.h"
#include "ecs/scheduler.h"
#include "systems/entitySystem.h"
#include "systems/testCompSystem.h"
#include "util/atomics.h"
#include "util/linalloc.h"
#include "util/sync.h"

void printColumn(ECTColumn(Entity)* column)
{
    printf("%u\n", column->components.size);
    for(uint32_t i = 0; i < column->components.size; ++i)
    {
        printf("{%08x@%08x:%016lx}\n", column->components.data[i].self.id, column->components.data[i].self.parent, column->components.data[i].componentMask[0]);
    }
    
    puts("");
}

void testECT()
{
    ECTable table;
    ECTColumn(Entity)* column;
    
    Entity e = {};
    Entity c = {};
    TestComp t = {};
    ObjectID id;
    
    puts("Testing ECTable");
    
    e.transform = TRANSFORM_IDENTITY;
    e.self.parent = INVALID_OBJECT;
    
    c.transform = TRANSFORM_IDENTITY;
    
    t.number = 0xdeadbeef;
    
    ecTableCreate(&table, NUM_COMPONENT_TYPES);
    column = (ECTColumn(Entity)*)&table.columns[0];
    
    ectColumnCreate(Entity)(column);
    ectColumnCreate(TestComp)((ECTColumn(TestComp)*)&table.columns[1]);
    printColumn(column);
    
    ecTableAdd(&table, Entity, &e, INVALID_OBJECT, &id);
    ecTableAdd(&table, Entity, &c, id, &id);
    ecTableAdd(&table, TestComp, &t, id, &id);
    
    ecTableMark(&table);
    ecTableAddRemove(&table);
    printColumn(column);
    
    ecTableRemove(&table, TestComp, t.self.id);
    ecTableRemove(&table, Entity, c.self.id);
    
    ecTableMark(&table);
    ecTableAddRemove(&table);
    printColumn(column);
    
    ecTableRemove(&table, Entity, e.self.id);
    ecTableMark(&table);
    ecTableAddRemove(&table);
    printColumn(column);
    
    ecTableDestroy(&table);
    
    puts("Done ECTable");
}

int32_t testLinalloc()
{
    uint8_t buffer[1024];
    uint32_t* testPtr = NULL;
    
    printf("Testing linalloc\nBuffer start %lx\n", buffer);
    
    linInit(buffer, 1024);
    
    while(1)
    {
        testPtr = (uint32_t*)linalloc(24);
        printf("%lx ", testPtr);
        if(!testPtr) break;
        *testPtr = 1;
    }
    
    puts("");
    
    linReset();
    
    while(1)
    {
        testPtr = (uint32_t*)linalloc(24);
        printf("%lx ", testPtr);
        if(!testPtr) break;
        *testPtr = 2;
    }
    puts("");
    
    linCleanup();
    
    puts("Done linalloc");
}

void* sceneMulti(void* running)
{
    SceneManager* sceneMan = sceneManagerGetInstance();
    
    while(atomicLoad32((volatile uint32_t*)running))
    {
        sceneManagerFollowFrame(sceneMan, 0.0f);
    }
    
    return NULL;
}

void testSceneMan(int32_t numThreads)
{
    uint8_t buffer[1024];
    pthread_t helpers[numThreads - 1];
    
    SceneManager* sceneMan = sceneManagerGetInstance();
    
    ECTable* table;
    Entity e = {};
    Entity c = {};
    TestComp t = {};
    ObjectID eid;
    ObjectID cid;
    
    volatile uint32_t running;
    atomicStore32(&running, 1);
    
    puts("Testing SceneManager");
    
    linInit(buffer, 1024);
    
    e.transform = TRANSFORM_IDENTITY;
    e.self.parent = INVALID_OBJECT;
    tagSet(&e.name, "Parent");
    
    c.transform = TRANSFORM_IDENTITY;
    tagSet(&c.name, "Child");
    
    sceneManagerCreate(sceneMan, numThreads, numThreads);
    sceneManagerRegisterColumnSys(sceneMan, &ENTITY_SYSTEM, COMPONENT(Entity));
    sceneManagerRegisterColumnSys(sceneMan, &TEST_COMP_SYSTEM, COMPONENT(TestComp));
    sceneManagerInit(sceneMan);
    
    table = sceneManagerGetTable(sceneMan);
    ecTableAdd(table, Entity, &e, INVALID_OBJECT, &eid);
    ecTableAdd(table, Entity, &c, eid, &cid);
    ecTableAdd(table, TestComp, &t, eid, &cid);
    
    for(int32_t i = 0; i < numThreads - 1; ++i)
    {
        pthread_create(&helpers[i], NULL, sceneMulti, (void*)&running);
    }
    
    for(uint32_t i = 0; i < 32; ++i)
    {
        printf("Frame %u\n", i);
        sceneManagerFrame(sceneMan, 0.0f, i == 32 - 1);
    }
    
    atomicStore32(&running, 0);
    
    for(int32_t i = 0; i < numThreads - 1; ++i)
    {
        pthread_join(helpers[i], NULL);
    }
    
    sceneManagerDestroy(sceneMan);
    linCleanup();
    
    puts("Done SceneManager");
}

typedef struct
{
    Scheduler* scheduler;
    volatile uint32_t* counters;
    Barrier* barrier1;
    Barrier* barrier2;
    volatile uint32_t* running;
}
TestArgs;

void* schedMultiTest(void* a)
{
    TestArgs* args = (TestArgs*)a;
    Job j;
    uint64_t idx = 0;
    
    while(atomicLoad32(args->running))
    {
        barrierWait(args->barrier1);
        
        while(schedulerGetNext(args->scheduler, &j))
        {
            idx = (uint64_t)j.args;
            fetchAdd32(&args->counters[idx], 1);
        }
        
        barrierWait(args->barrier2);
    }
    
    return NULL;
}

#define NUM_THREADS 4

void testSchedulerMulti()
{
    Scheduler sched;
    volatile uint32_t counters[16] = {};
    Barrier barrier1;
    Barrier barrier2;
    volatile uint32_t running = 1;
    
    JobFun jf;
    
    Job j;
    uint64_t idx;
    
    puts("Testing scheduler");
    
    pthread_t threads[NUM_THREADS - 1];
    
    TestArgs args = {&sched, counters, &barrier1, &barrier2, &running};
    
    schedulerCreate(&sched);
    
    barrierCreate(&barrier1, NUM_THREADS);
    barrierCreate(&barrier2, NUM_THREADS);
    
    puts("Registering functions");
    jf.genericFun = NULL;
    for(uint64_t i = 0; i < 16; ++i)
    {
        schedulerRegister(&sched, jf, JOB_TYPE(compReady), (void*)i);
    }
    schedulerReset(&sched);
    
    for(uint64_t i = 0; i < 16; ++i)
    {
        printf("%lx\n", sched.queue.data[i].args);
    }
    puts("Runing schedule");
    
    for(uint32_t i = 0; i < NUM_THREADS - 1; ++i)
    {
        pthread_create(&threads[i], NULL, schedMultiTest, &args);
    }
    
    for(uint32_t i = 0; i < 65534; ++i)
    {
        barrierWait(&barrier1);
        
        while(schedulerGetNext(&sched, &j))
        {
            idx = (uint64_t)j.args;
            fetchAdd32(&counters[idx],1);
        }
        
        barrierWait(&barrier2);
        schedulerReset(&sched);
    }
    
    barrierWait(&barrier1);
    while(schedulerGetNext(&sched, &j))
    {
        idx = (uint64_t)j.args;
        fetchAdd32(&counters[idx], 1);
    }
    
    atomicStore32(&running, 0);
    barrierWait(&barrier2);
    
    puts("Getting results");
    
    for(uint32_t i = 0; i < NUM_THREADS - 1; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    
    for(uint32_t i = 0; i < 16; ++i)
    {
        printf("%x\n", counters[i]);
    }
    
    puts("Cleaning up");
    
    barrierDestroy(&barrier2);
    barrierDestroy(&barrier1);
    
    schedulerDestroy(&sched);
    
    puts("Done Scheduler");
}

int32_t main()
{
    testECT();
    puts("");
    
    testLinalloc();
    puts("");
    
    testSceneMan(2);
    puts("");
    
    testSchedulerMulti();
    puts("");
    
    return 0;
}
