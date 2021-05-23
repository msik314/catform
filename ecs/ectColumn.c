#include "util/globalDefs.h"
#include "ecs/ectColumn.h"

#include <stdint.h>
#include "containers/collection.h"
#include "containers/mwQueue.h"

void ectColumnDestroyGeneric(ECTColumn* ectColumn)
{
    mwQueueDestroyGeneric(&ectColumn->addQueue);\
    collectionDestroyGeneric(&ectColumn->components);\
}
