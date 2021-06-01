#include "util/globalDefs.h"
#include "systems/testCompSystem.h"

#include <string.h>
#include <stdio.h>

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/object.h"
#include "ecs/sceneManager.h"
#include "components/component.h"
#include "components/entity.h"
#include "components/testComp.h"
#include "util/linalloc.h"

static void testCompColCreate(ECTColumn* column) {ectColumnCreate(TestComp)((ECTColumn(TestComp)*)column);}

const ECSystem TEST_COMP_SYSTEM =
{
    testCompSysInit,
    testCompSysDestroy,
    
    testCompColCreate,
    
    testCompCompReady,
    testCompSysFlags,
    testCompCompUpdate,
    testCompCompDestroy,
    
    testCompCompReadyAll,
    testCompCompDestroyAll
};

void testCompSysInit(ECSystem* self){}

void testCompSysDestroy(ECSystem* self){}

void testCompCompReady(ECSystem* self, ECTColumn* column)
{
    ECTColumn(TestComp)* components = (ECTColumn(TestComp)*)column;
    uint32_t idx = components->components.size;
    uint32_t flags;
    
    while(idx)
    {
        --idx;
        
        flags = atomicLoad32(&components->components.data[idx].self.flags);
        if(!(flags & OBJECT_FLAG_UNREADY))
        {
            return;
        }
        
        fetchAnd32(&components->components.data[idx].self.flags, ~OBJECT_FLAG_UNREADY);
        testCompReady(&components->components.data[idx]);
    }
}

void testCompSysFlags(ECSystem* self, void** flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime)
{
    const ECTColumn(TestComp)* components = (ECTColumn(TestComp)*)&columns[COMPONENT(TestComp)];
    const ECTColumn* entities = &columns[COMPONENT(Entity)];
    const PointerMap* map = sceneManagerGetMap(sceneManagerGetInstance());
    
    bool* componentFlags = (bool*)linalloc(entities->components.size * sizeof(bool));
    memset(componentFlags, false, entities->components.size * sizeof(bool));
    
    for(uint32_t i = 0; i < components->components.size; ++i)
    {
        if(components->components.data[i].number >= 15)
        {
            componentFlags[pointerMapGet(map, components->components.data[i].self.parent)] = true;
        }
    }
    *flags = componentFlags;
}

void testCompCompUpdate(ECSystem* self, ECTColumn* column, const void** flags, uint32_t numFlags, float deltaTime)
{
    ECTColumn(TestComp)* components = (ECTColumn(TestComp)*)column;
    
    for(uint32_t i = 0; i < components->components.size; ++i)
    {
        printf("%u\n", components->components.data[i].number);
        components->components.data[i].number += 1;
    }
}

void testCompCompDestroy(ECSystem* self, ECTColumn* column)
{
    ECTColumn(TestComp)* components = (ECTColumn(TestComp)*)column;
    uint32_t flags;
    
    for(uint32_t i = 0; i < components->components.size; ++i)
    {
        flags = atomicLoad32(&components->components.data[i].self.flags);
        if(flags & OBJECT_FLAG_REMOVE) testCompOnDestroy(&components->components.data[i]);
    }
}

void testCompCompReadyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(TestComp)* components = (ECTColumn(TestComp)*)column;
    uint32_t flags;
    
    for(uint32_t i = 0; i < components->components.size; ++i)
    {
        fetchAnd32(&components->components.data[i].self.flags, ~OBJECT_FLAG_UNREADY);
        testCompReady(&components->components.data[i]);
    }
}

void testCompCompDestroyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(TestComp)* components = (ECTColumn(TestComp)*)column;
    TestComp tc;
    
    for(uint32_t i = 0; i < components->components.size; ++i)
    {
        testCompOnDestroy(&components->components.data[i]);
    }
    
    while(mwQueuePop(TestComp)(&components->addQueue, &tc))
    {
        testCompOnDestroy(&tc);
    }
}

void testCompReady(TestComp* testComp)
{
    testComp->number = 0;
}

void testCompOnDestroy(TestComp* testComp)
{
    
}
