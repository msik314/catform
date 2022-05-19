#ifndef ECTCOLUMN_H
#define ECTCOLUMN_H

#include "containers/collection.h"
#include "containers/hashmap.h"
#include "containers/mwQueue.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "json/jsonData.h"

#define INVALID_COLUMN_INDEX 0xffffffff

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus


struct _ECTColumn;

typedef void (*ECTColumnParentFun)(struct _ECTColumn* self, struct _ECTColumn* entites, PointerMap* pointerMap);
typedef void (*ECTColumnAddRemoveFun)(struct _ECTColumn* self, PointerMap* pointerMap);

typedef void (*ECTColumnSerializeFun)(const struct _ECTColumn* self, JsonData* data, uint32_t column);
typedef void (*ECTColumnDeserializeFun)(struct _ECTColumn* self, const JsonData* data, uint32_t column, const Hashmap(ObjectID, ObjectID)* refMap);

typedef void (*ECTColumnGetIDsFun)(const struct _ECTColumn* self, ObjectID* outIds);
typedef void (*ECTColumnAddAllFun)(struct _ECTColumn* dst, const struct _ECTColumn* src, const Hashmap(ObjectID, ObjectID)* refMap);

typedef struct _ECTColumn
{
    Collection components;
    MWQueue addQueue;
}
ECTColumn;

void ectColumnDestroyGeneric(ECTColumn* ectColumn);

#define ECTColumn(TYPE) ECTColumn ## TYPE
#define ectColumnCreate(TYPE) ectColumn ## TYPE ## Create
#define ectColumnDestroy(TYPE) ectColumn ## TYPE ## Destroy
#define ectColumnAdd(TYPE) ectColumn ## TYPE ## Add
#define ectColumnAddID(TYPE) ectColumn ## TYPE ## AddID
#define ectColumnRemove(TYPE) ectColumn ## TYPE ## Remove
#define ectColumnAddRemove(TYPE) ectColumn ## TYPE ## AddRemove
#define ectColumnRemoveAll(TYPE) ectColumn ## TYPE ## RemoveAll
#define ectColumnParentDelete(TYPE) ectColumn ## TYPE ## ParentDelete
#define ectColumnParentAdd(TYPE) ectColumn ## TYPE ## ParentAdd
#define ectColumnGetIDs(TYPE) ectColumn ## TYPE ## GetIDs
#define ectColumnAddAll(TYPE) ectColumn ## TYPE ## AddAll

#define ECTCOLUMN_DECL(TYPE) \
typedef struct\
{\
    Collection(TYPE) components;\
    MWQueue(TYPE) addQueue;\
}\
ECTColumn(TYPE);\
\
void ectColumnCreate(TYPE)(ECTColumn(TYPE)* ectColumn);\
void ectColumnDestroy(TYPE)(ECTColumn(TYPE)* ectColumn);\
void ectColumnAdd(TYPE)(ECTColumn(TYPE)* ectColumn, TYPE* original);\
ObjectID ectColumnAddID(TYPE)(ECTColumn(TYPE)* ectColumn, TYPE* original, PointerMap* pointerMap);\
bool ectColumnRemove(TYPE)(ECTColumn(TYPE)* ectColumn, uint32_t index);\
void ectColumnAddRemove(TYPE)(ECTColumn* ectColumnGen, PointerMap* pointerMap);\
void ectColumnRemoveAll(TYPE)(ECTColumn* ectColumnGen, PointerMap* pointerMap);\
void ectColumnParentDelete(TYPE)(ECTColumn* ectColumnGen, ECTColumn* entitiesGen, PointerMap* pointerMap);\
void ectColumnParentAdd(TYPE)(ECTColumn* ectColumnGen, ECTColumn* entitiesGen, PointerMap* pointerMap);\
void ectColumnGetIDs(TYPE)(const ECTColumn* ectColumnGen, ObjectID* outIds);\
void ectColumnAddAll(TYPE)(ECTColumn* dstGen, const ECTColumn* srcGen, const Hashmap(ObjectID, ObjectID)* refMap);

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //ECTCOLUMN_H
