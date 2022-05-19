#include "util/globalDefs.h"
#include "ecs/ectColumn.h"

#include <stdint.h>
#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "components/component.h"
#include "components/entity.h"
#include "ecs/ectVirtualTable.h"
#include "util/atomics.h"

#define ECTCOLUMN_IMPL_FUNCTIONS(TYPE, ADD_REMOVE, REMOVE_ALL, PARENT_ADD, PARENT_DELETE, SERIALIZE, DESERIALIZE) \
void ectColumnCreate(TYPE)(ECTColumn(TYPE)* ectColumn)\
{\
    collectionCreate(TYPE)(&ectColumn->components);\
    mwQueueCreate(TYPE)(&ectColumn->addQueue);\
    if(!getVirtualAddRemove(COMPONENT(TYPE)))\
    {\
        setVirtualAddRemove(COMPONENT(TYPE), ADD_REMOVE);\
        setVirtualRemoveAll(COMPONENT(TYPE), REMOVE_ALL);\
        setVirtualParentDelete(COMPONENT(TYPE), PARENT_DELETE);\
        setVirtualParentAdd(COMPONENT(TYPE), PARENT_ADD);\
        setVirtualSerialize(COMPONENT(TYPE), SERIALIZE);\
        setVirtualDeserialize(COMPONENT(TYPE), DESERIALIZE);\
        setVirtualGetIDs(COMPONENT(TYPE), ectColumnGetIDs(TYPE));\
        setVirtualAddAll(COMPONENT(TYPE), ectColumnAddAll(TYPE));\
    }\
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
void ectColumnRemoveAll(TYPE)(ECTColumn* ectColumnGen, PointerMap* pointerMap)\
{\
    ECTColumn(TYPE)* ectColumn = (ECTColumn(TYPE)*)ectColumnGen;\
    Object* obj;\
    uint32_t idx;\
    ObjectID oldId;\
    \
    idx = ectColumn->components.size;\
    \
    while(idx)\
    {\
        --idx;\
        obj = (Object*)&ectColumn->components.data[idx];\
        oldId = obj->id;\
        pointerMapRemove(pointerMap, oldId);\
        collectionRemove(TYPE)(&ectColumn->components, idx);\
    }\
}\
\
void ectColumnAddRemove(TYPE)(ECTColumn* ectColumnGen, PointerMap* pointerMap)\
{\
    ECTColumn(TYPE)* ectColumn = (ECTColumn(TYPE)*)ectColumnGen;\
    Object* obj;\
    uint32_t idx;\
    ObjectID oldId;\
    ObjectID newId;\
    TYPE tmp;\
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
        if(tmp.self.id == INVALID_OBJECT)\
        {\
            tmp.self.id = pointerMapAdd(pointerMap, ectColumn->components.size);\
        }\
        else\
        {\
            pointerMapSet(pointerMap, tmp.self.id, ectColumn->components.size);\
        }\
        \
        collectionAdd(TYPE)(&ectColumn->components, &tmp);\
    }\
}\
\
void ectColumnParentDelete(TYPE)(ECTColumn* ectColumnGen, ECTColumn* entitiesGen, PointerMap* pointerMap)\
{\
    ECTColumn(TYPE)* ectColumn = (ECTColumn(TYPE)*)ectColumnGen;\
    ECTColumn(Entity)* entities = (ECTColumn(Entity)*)entitiesGen;\
    Entity* parent;\
    Object* obj;\
    uint32_t flags;\
    \
    for(uint32_t i = 0; i < ectColumn->components.size; ++i)\
    {\
        obj = (Object*)&ectColumn->components.data[i];\
        if(obj->parent == INVALID_OBJECT) continue;\
        parent = &entities->components.data[pointerMapGet(pointerMap, obj->parent)];\
        \
        flags = fetchOr32(&obj->flags, atomicLoad32(&parent->self.flags) & OBJECT_FLAG_REMOVE);\
        if(flags & OBJECT_FLAG_REMOVE) entityResetHasComponent(parent, COMPONENT(TYPE));\
    }\
}\
\
void ectColumnParentAdd(TYPE)(ECTColumn* ectColumnGen, ECTColumn* entitiesGen, PointerMap* pointerMap)\
{\
    ECTColumn(TYPE)* ectColumn = (ECTColumn(TYPE)*)ectColumnGen;\
    ECTColumn(Entity)* entities = (ECTColumn(Entity)*)entitiesGen;\
    Entity* parent;\
    Object* obj;\
    uint32_t idx;\
    \
    idx = ectColumn->components.size;\
    \
    while(idx)\
    {\
        --idx;\
        obj = (Object*)&ectColumn->components.data[idx];\
        if(!(atomicLoad32(&obj->flags) & OBJECT_FLAG_UNREADY)) break;\
        if(obj->parent == INVALID_OBJECT) continue;\
        parent = &entities->components.data[pointerMapGet(pointerMap, obj->parent)];\
        entitySetHasComponent(parent, COMPONENT(TYPE));\
    }\
}\
\
void ectColumnGetIDs(TYPE)(const ECTColumn* ectColumnGen, ObjectID* outIds)\
{\
    const ECTColumn(TYPE)* ectColumn = (const ECTColumn(TYPE)*)ectColumnGen;\
    for(uint32_t i = 0; i < ectColumn->components.size; ++i)\
    {\
        outIds[i] = ectColumn->components.data[i].self.id;\
    }\
}\
\
void ectColumnAddAll(TYPE)(ECTColumn* dstGen, const ECTColumn* srcGen, const Hashmap(ObjectID, ObjectID)* refMap)\
{\
    const ECTColumn(TYPE)* src = (const ECTColumn(TYPE)*)srcGen;\
    ECTColumn(TYPE)* dst = (ECTColumn(TYPE)*)dstGen;\
    TYPE t;\
    ObjectID translatedId;\
    \
    for(uint32_t i = 0; i < src->components.size; ++i)\
    {\
        t = src->components.data[i];\
        hashmapGet(ObjectID, ObjectID)(refMap, &t.self.id, &translatedId);\
        t.self.id = translatedId;\
        hashmapGet(ObjectID, ObjectID)(refMap, &t.self.parent, &translatedId);\
        t.self.parent = translatedId;\
        \
        ectColumnAdd(TYPE)(dst, &t);\
    }\
}\

#define ECTCOLUMN_IMPL_SER(TYPE, SERIALIZE, DESERIALIZE)\
ECTCOLUMN_IMPL_FUNCTIONS(TYPE, ectColumnAddRemove(TYPE), ectColumnRemoveAll(TYPE), ectColumnParentAdd(TYPE), ectColumnParentDelete(TYPE), SERIALIZE, DESERIALIZE)

#define ECTCOLUMN_IMPL(TYPE) ECTCOLUMN_IMPL_SER(TYPE, NULL, NULL)
