#include "util/globalDefs.h"
#include "ecs/ecTable.h"

#include <string.h>
#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "ecs/pointerMap.h"
#include "ecs/component.h"

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

