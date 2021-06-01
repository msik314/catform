#include <stdio.h>
#include <stdint.h>

#include "core/tag.h"
#include "components/entity.h"
#include "components/testComp.h"
#include "ecs/ectColumn.h"
#include "ecs/pointerMap.h"
#include "ecs/ecTable.h"
#include "ecs/sceneManager.h"
#include "systems/entitySystem.h"
#include "systems/testCompSystem.h"
#include "util/linalloc.h"

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

void testSceneMan()
{
    uint8_t buffer[1024];
    SceneManager* sceneMan = sceneManagerGetInstance();
    
    ECTable* table;
    Entity e = {};
    Entity c = {};
    TestComp t = {};
    ObjectID eid;
    ObjectID cid;
    
    puts("Testing SceneManager");
    
    linInit(buffer, 1024);
    
    e.transform = TRANSFORM_IDENTITY;
    e.self.parent = INVALID_OBJECT;
    tagSet(&e.name, "Parent");
    
    c.transform = TRANSFORM_IDENTITY;
    tagSet(&c.name, "Child");
    
    sceneManagerCreate(sceneMan);
    sceneManagerRegisterColumnSys(sceneMan, &ENTITY_SYSTEM, COMPONENT(Entity));
    sceneManagerRegisterColumnSys(sceneMan, &TEST_COMP_SYSTEM, COMPONENT(TestComp));
    sceneManagerInit(sceneMan);
    
    table = sceneManagerGetTable(sceneMan);
    ecTableAdd(table, Entity, &e, INVALID_OBJECT, &eid);
    ecTableAdd(table, Entity, &c, eid, &cid);
    ecTableAdd(table, TestComp, &t, eid, &cid);
    
    for(uint32_t i = 0; i < 32; ++i)
    {
        printf("Frame %u\n", i);
        sceneManagerFrame(sceneMan, 0.0f);
    }
    
    sceneManagerDestroy(sceneMan);
    linCleanup();
    
    puts("Done SceneManager");
}

int32_t main()
{
    testECT();
    testLinalloc();
    testSceneMan();
    
    return 0;
}
