#ifndef ECTABLE_H
#define ECTABLE_H

#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "ecs/pointerMap.h"
#include "components/component.h"

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

#define ecTableAdd(table, type, objectPtr, parentID, outID)\
do\
{\
    (objectPtr)->self.parent = parentID;\
    *(outID) = ectColumnAddID(type)((ECTColumn(type)*)&(table)->columns[COMPONENT(type)], (objectPtr), &(table)->pointerMap);\
}\
while(0);

#define ecTableRemove(table, type, objectID) (ectColumnRemove(type)((ECTColumn(type)*)&(table)->columns[COMPONENT(type)], pointerMapGet(&(table)->pointerMap, objectID)))

#ifdef __cplusplus
};
#endif //__cplusplus


#endif //ECTABLE_H
