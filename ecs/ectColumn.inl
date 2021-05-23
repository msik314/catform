#include "util/globalDefs.h"
#include "ecs/ectColumn.h"

#include <stdint.h>
#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "util/atomics.h"

#define ECTCOLUMN_IMPL(TYPE) \
void ectColumnCreate(TYPE)(ECTColumn(TYPE)* ectColumn)\
{\
    collectionCreate(TYPE)(&ectColumn->components);\
    mwQueueCreate(TYPE)(&ectColumn->addQueue);\
}\
\
void ectColumnDestroy(TYPE)(ECTColumn(TYPE)* ectColumn)\
{\
    mwQueueDestroy(TYPE)(&ectColumn->addQueue);\
    collectionDestroy(TYPE)(&ectColumn->components);\
}\
\
void ectColumnAdd(TYPE)(ECTColumn(TYPE)* ectColumn, TYPE* original)\
{\
    original->self.id = INVALID_OBJECT;\
    mwQueuePush(TYPE)(&ectColumn->addQueue, original);\
}\
\
ObjectID ectColumnAddID(TYPE)(ECTColumn(TYPE)* ectColumn, TYPE* original, PointerMap* pointerMap)\
{\
    original->self.id = pointerMapAdd(pointerMap, INVALID_COLUMN_INDEX);\
    mwQueuePush(TYPE)(&ectColumn->addQueue, original);\
    return original->self.id;\
}\
\
bool ectColumnRemove(TYPE)(ECTColumn(TYPE)* ectColumn, uint32_t index)\
{\
    register Object* obj = (Object*)&ectColumn->components.data[index];\
    return fetchOr32(&obj->flags, OBJECT_FLAG_REMOVE) & OBJECT_FLAG_REMOVE;\
}\
\
void ectColumnAddRemove(TYPE)(ECTColumn(TYPE)* ectColumn, PointerMap* pointerMap)\
{\
    TYPE tmp;\
    uint32_t idx;\
    ObjectID oldId;\
    ObjectID newId;\
    register Object* obj;\
    \
    idx = ectColumn->components.size;\
    \
    while(idx)\
    {\
        --idx;\
        obj = (Object*)&ectColumn->components.data[idx];\
        if(atomicLoad32(&obj->flags) & OBJECT_FLAG_REMOVE)\
        {\
            oldId = obj->id;\
            pointerMapRemove(pointerMap, oldId);\
            collectionRemove(TYPE)(&ectColumn->components, idx);\
            \
            obj = (Object*)&ectColumn->components.data[idx];\
            newId = obj->id;\
            if(newId != oldId)\
            {\
                pointerMapSet(pointerMap, newId, idx);\
            }\
        }\
    }\
    \
    while(mwQueuePop(TYPE)(&ectColumn->addQueue, &tmp))\
    {\
        fetchOr32(&tmp.self.flags, OBJECT_FLAG_UNREADY);\
        if(tmp.self.id == INVALID_COLUMN_INDEX)\
        {\
            tmp.self.id = pointerMapAdd(pointerMap, ectColumn->components.size);\
        }\
        else\
        {\
            pointerMapSet(pointerMap, tmp.self.id, ectColumn->components.size);\
        }\
        collectionAdd(TYPE)(&ectColumn->components, &tmp);\
    }\
}
