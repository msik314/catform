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
#include "util/atomics.h"
#include "util/linalloc.h"

static void testCompColCreate(ECTColumn* column) {ectColumnCreate(TestComp)((ECTColumn(TestComp)*)column);}

const ECSystem TEST_COMP_SYSTEM =
{
    testCompSysInit,
    testCompSysDestroy,
    
    testCompColCreate,
    
    testCompCompReady,
    testCompSysUpdate,
    testCompCompCopy,
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

void testCompSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime)
{
    const ECTColumn(TestComp)* components = (ECTColumn(TestComp)*)&columns[COMPONENT(TestComp)];
    const PointerMap* map = sceneManagerGetMap(sceneManagerGetInstance());
    uint32_t data;
    
    uint32_t* componentFlags = (uint32_t*)linalloc((2 * components->components.size + 1) * sizeof(uint32_t));
    
    componentFlags[0] = components->components.size;
    
    for(uint32_t i = 0; i < components->components.size; ++i)
    {
        componentFlags[2 * i + 1] = components->components.data[i].number + 1;
        componentFlags[2 * i + 2] = pointerMapGet(map, components->components.data[i].self.parent);
    }
    atomicStorePtr(flags, componentFlags);
}

void testCompCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime)
{
    ECTColumn(TestComp)* components = (ECTColumn(TestComp)*)column;
    uint32_t* componentFlags = (uint32_t*)atomicLoadPtr(&flags[1]); //TestComp flags;
    
    for(uint32_t i = 0; i < components->components.size; ++i)
    {
        components->components.data[i].number = componentFlags[2 * i + 1];
        printf("%u\n", components->components.data[i].number);
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
