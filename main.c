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
#include "json/jsonData.h"
#include "json/jsonReader.h"
#include "json/jsonWriter.h"

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

void _assert(uint32_t expr, const char* str)
{
    int *a;
    if(!expr)
    {
        fputs(str, stderr);
        a = NULL;
        *a = 1;
    }
}


void testJSONRead()
{
    JsonData json;
    JsonObject* object;
    JsonType type;
    uint32_t objectIndex;
    int32_t intVal;
    bool boolVal;
    char* strVal;
    double realVal;
    char buffer[1000] = {};
    
    puts("Testing JSON loading");
    jsonLoadf(&json, "res/test.json");
    
    puts("Printing data");
    type = jsonObjectGetKey(&json.root, "object", &objectIndex);
    _assert(type == JSON_TYPE_OBJECT, "\"object\" not of type Object");
    object = jsonDataGetChild(&json, objectIndex);
    
    type = jsonObjectGetKey(object, "real", &realVal);
    _assert(type == JSON_TYPE_REAL, "\"object.real\" not of type Real");
    printf("object.real: %.2f\n", realVal);
    
    type = jsonObjectGetKey(object, "int", &intVal);
    _assert(type == JSON_TYPE_INT, "\"object.int\" not of type Int");
    printf("object.int: %d\n", intVal);
    
    type = jsonObjectGetKey(object, "bool", &boolVal);
    _assert(type == JSON_TYPE_BOOL, "\"object.bool\" not of type Bool");
    printf("object.bool: %x\n", boolVal);
    
    type = jsonObjectGetKey(&json.root, "array", &objectIndex);
    _assert(type == JSON_TYPE_ARRAY, "\"array\" not of type Array");
    object = jsonDataGetChild(&json, objectIndex);
    for(uint32_t i = 0; i < jsonArraySize(object); ++i)
    {
        type = jsonArrayGetIndex(object, i, &intVal);
        printf("array[%d]: %d\n", i, intVal);
    }
    
    type = jsonObjectGetKey(&json.root, "string", &strVal);
    _assert(type == JSON_TYPE_STRING, "\"string\" not of type String");
    printf("string: %s\n", strVal);
    
    puts("Dumping json");
    intVal = jsonWrites(&json, buffer, sizeof(buffer)/sizeof(char));
    printf("Json string with length %d:\n%s\n", intVal, buffer);
    
    puts("Cleaning up");
    jsonDataDestroy(&json);
    
    puts("Done JSON loading");
}

void testJSONWrite()
{
    char buffer[1024] = {};
    const int32_t jsonArrayData[6] = {2, 4, 6, 5, 7, 3};
    JsonData data;
    JsonValue val;
    Tag tag;
    uint32_t parent;
    const char* str = "I am a string";
    
    puts("Testing JSON writing");
    
    jsonDataCreate(&data);
    
    tagSet(&tag, "string");
    val.stringValue = (char*)str;
    val.type = JSON_TYPE_STRING;
    jsonDataAddValue(&data, JSON_DATA_ROOT_INDEX, tag, val);
    
    tagSet(&tag, "array");
    parent = jsonDataAddArray(&data, JSON_DATA_ROOT_INDEX, tag);
    
    val.type = JSON_TYPE_INT;
    for(uint32_t i = 0; i < sizeof(jsonArrayData) / sizeof(int32_t); ++i)
    {
        val.intValue = jsonArrayData[i];
        jsonDataArrayAddValue(&data, parent, val);
    }
    
    tagSet(&tag, "object");
    parent = jsonDataAddObject(&data, JSON_DATA_ROOT_INDEX, tag);
    
    tagSet(&tag, "bool");
    val.boolValue = false;
    val.type = JSON_TYPE_BOOL;
    jsonDataAddValue(&data, parent, tag, val);
    
    tagSet(&tag, "real");
    val.realValue = 3.141592;
    val.type = JSON_TYPE_REAL;
    jsonDataAddValue(&data, parent, tag, val);
    
    tagSet(&tag, "int");
    val.intValue = 65535;
    val.type = JSON_TYPE_INT;
    jsonDataAddValue(&data, parent, tag, val);
    
    jsonWrites(&data, buffer, 1024);
    puts(buffer);
    jsonDataDestroy(&data);
    
    puts("Done JSON writing");
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
    
    testJSONRead();
    puts("");
    
    testJSONWrite();
    
    return 0;
}
