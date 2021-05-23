#include <stdio.h>
#include <stdint.h>
#include "ecs/entity.h"
#include "ecs/ectColumn.h"
#include "ecs/pointerMap.h"
#include "ecs/ecTable.h"
#include "util/linalloc.h"

#include "ecs/ectColumn.inl"
ECTCOLUMN_IMPL(Entity);

void printColumn(ECTColumn(Entity)* column)
{
    printf("%u\n", column->components.size);
    for(uint32_t i = 0; i < column->components.size; ++i)
    {
        printf("{%08x@%08x}\n", column->components.data[i].self.id, column->components.data[i].self.parent);
    }
    
    puts("");
}

int32_t main()
{
    uint8_t buffer[1024];
    
    ECTable table;
    ECTColumn(Entity)* column;
    
    Entity e;
    Entity c;
    uint32_t idx;
    uint32_t* testPtr = NULL;
    
    e.transform = TRANSFORM_IDENTITY;
    e.self.parent = INVALID_OBJECT;
    
    c.transform = TRANSFORM_IDENTITY;
    
    ecTableCreate(&table, 1);
    column = (ECTColumn(Entity)*)&table.columns[0];
    
    ectColumnCreate(Entity)(column);
    printColumn(column);      
    
    ecTableAdd(Entity)(&table, &e, INVALID_OBJECT);
    
    ecTableAdd(Entity)(&table, &c, e.self.id);
    ectColumnAddRemove(Entity)(column, &table.pointerMap);
    printColumn(column);
    
    idx = pointerMapGet(&table.pointerMap, c.self.id);
    ecTableRemove(Entity)(&table, idx);
    
    ectColumnAddRemove(Entity)(column, &table.pointerMap);
    printColumn(column);
    
    idx = pointerMapGet(&table.pointerMap, e.self.id);
    ecTableRemove(Entity)(&table, idx);
    ectColumnAddRemove(Entity)(column, &table.pointerMap);
    printColumn(column);
    
    ecTableDestroy(&table);
    
    puts("Done ECTable");
    
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
    
    return 0;
}
