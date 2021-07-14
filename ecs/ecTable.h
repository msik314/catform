#ifndef ECTABLE_H
#define ECTABLE_H

#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "ecs/pointerMap.h"
#include "components/component.h"
#include "json/jsonData.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    ECTColumn* columns;
    uint32_t numColumns;
    PointerMap pointerMap;
}
ECTable;

void ecTableCreate(ECTable* table, uint32_t numColumns);
void ecTableDestroy(ECTable* table);
void ecTableMark(ECTable* table);
void ecTableAddRemove(ECTable* table);
void ecTableRemoveAll(ECTable* table);
uint32_t ecTableGetChildren(const ECTable* table, ObjectID entity, ObjectID* outEntities, uint32_t maxOut);

uint32_t ecTableGetMarkFuns(const ECTable* table, ECTColumnParentFun* outFuns, uint32_t maxFuns);
uint32_t ecTableGetARFuns(const ECTable* table, ECTColumnAddRemoveFun* outFuns, uint32_t maxFuns);
uint32_t ecTableGetParentFuns(const ECTable* table, ECTColumnParentFun* outFuns, uint32_t maxFuns);

ObjectID ecTableMap(ECTable* table, Object* object);

#define ecTableAdd(table, type, objectPtr, parentID, outID)\
do\
{\
    (objectPtr)->self.parent = parentID;\
    *(outID) = ectColumnAddID(type)((ECTColumn(type)*)&(table)->columns[COMPONENT(type)], (objectPtr), &(table)->pointerMap);\
}\
while(0)

#define ecTableAddNoMap(table, type, objectPtr, parentID)\
do\
{\
    (objectPtr)->self.parent = parentID;\
    ectColumnAdd(type)((ECTColumn(type)*)&(table)->columns[COMPONENT(type)], (objectPtr));\
}\
while(0)

#define ecTableRemove(table, type, objectID) (ectColumnRemove(type)((ECTColumn(type)*)&(table)->columns[COMPONENT(type)], pointerMapGet(&(table)->pointerMap, objectID)))

void ecTableSerialize(const ECTable* table, JsonData* data, uint32_t parentObject);
void ecTableDeserialize(ECTable* table, const JsonData* data, uint32_t parentObject);

#ifdef __cplusplus
};
#endif //__cplusplus


#endif //ECTABLE_H
