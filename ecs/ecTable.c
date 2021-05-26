#include "util/globalDefs.h"
#include "ecs/ecTable.h"

#include <string.h>
#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "ecs/pointerMap.h"
#include "ecs/component.h"
#include "ecs/entity.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

#define MAX_OBJECTS 4096

void ecTableCreate(ECTable* table, uint32_t numColumns)
{
    table->columns = (ECTColumn*)CAT_MALLOC(numColumns * sizeof(ECTColumn));
    table->numColumns = numColumns;
    
    memset(table->columns, 0, numColumns * sizeof(ECTColumn));
    
    pointerMapCreate(&table->pointerMap, MAX_OBJECTS);
}

void ecTableDestroy(ECTable* table)
{
    pointerMapDestroy(&table->pointerMap);
    
    for(uint32_t i = 0; i < table->numColumns; ++i)
    {
        ectColumnDestroyGeneric(&table->columns[i]);
    }
}

void ecTableAddRemove(ECTable* table)
{
    for(uint32_t i = 0; i < table->numColumns; ++i)
    {
        table->columns[i].addRemove(&table->columns[i], &table->columns[COMPONENT(Entity)], &table->pointerMap); 
    }
}

uint32_t ecTableGetChildren(const ECTable* table, ObjectID entity, ObjectID* outEntities, uint32_t maxOut)
{
    uint32_t numFound = 0;
    for(uint32_t i = 0; i < table->columns[COMPONENT(Entity)].components.size; ++i)
    {
        if(((ECTColumn(Entity)*)&table->columns[COMPONENT(Entity)])->components.data[i].self.parent == entity)
        {
            outEntities[numFound] = ((ECTColumn(Entity)*)&table->columns[COMPONENT(Entity)])->components.data[i].self.id;
            ++numFound;
            if(numFound == maxOut) return numFound;
        }
    }
    
    return numFound;
}
