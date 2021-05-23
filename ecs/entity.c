#include "util/globalDefs.h"
#include "ecs/entity.h"

#include <stdint.h>
#include "cmath/cvec.h"
#include "cmath/transform.h"
#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "ecs/ecTable.h"
#include "ecs/component.h"

ObjectID ecTableAdd(Entity)(ECTable* table, Entity* original, ObjectID parent)
{
    return ectColumnAddID(Entity)((ECTColumn(Entity)*)&table->columns[COMPONENT(Entity)], original, &table->pointerMap);
}

bool ecTableRemove(Entity)(ECTable* table, ObjectID id)
{
    return ectColumnRemove(Entity)((ECTColumn(Entity)*)&table->columns[COMPONENT(Entity)], pointerMapGet(&table->pointerMap, id));
}

ObjectID ecTableGetComponent(Entity)(const ECTable* table, ObjectID entity) {return entity;}

uint32_t ecTableGetChildren(Entity)(const ECTable* table, ObjectID entity, ObjectID* outEntities, uint32_t maxOut)
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
