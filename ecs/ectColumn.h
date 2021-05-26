#ifndef ECTCOLUMN_H
#define ECTCOLUMN_H

#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"

#define INVALID_COLUMN_INDEX 0xffffffff

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus


struct _ECTColumn;

typedef void (*ECTColumnAddRemoveFun)(struct _ECTColumn* self, struct _ECTColumn* entities, PointerMap* pointerMap);

typedef struct _ECTColumn
{
    Collection components;
    MWQueue addQueue;
    ECTColumnAddRemoveFun addRemove;
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

#define ECTCOLUMN_DECL(TYPE) \
typedef struct\
{\
    Collection(TYPE) components;\
    MWQueue(TYPE) addQueue;\
    ECTColumnAddRemoveFun addRemove;\
}\
ECTColumn(TYPE);\
\
void ectColumnCreate(TYPE)(ECTColumn(TYPE)* ectColumn);\
void ectColumnDestroy(TYPE)(ECTColumn(TYPE)* ectColumn);\
void ectColumnAdd(TYPE)(ECTColumn(TYPE)* ectColumn, TYPE* original);\
ObjectID ectColumnAddID(TYPE)(ECTColumn(TYPE)* ectColumn, TYPE* original, PointerMap* pointerMap);\
bool ectColumnRemove(TYPE)(ECTColumn(TYPE)* ectColumn, uint32_t index);\
void ectColumnAddRemove(TYPE)(ECTColumn* ectColumnGen, ECTColumn* entites, PointerMap* pointerMap);

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //ECTCOLUMN_H
