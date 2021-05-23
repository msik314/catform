#ifndef ECTABLE_H
#define ECTABLE_H

#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "ecs/pointerMap.h"
#include "ecs/component.h"

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

#define ecTableAdd(TYPE) ecTable ## TYPE ## Add
#define ecTableRemove(TYPE) ecTable ## TYPE ## Remove
#define ecTableGetComponent(TYPE) ecTable ## TYPE ## GetComponent
#define ecTableGetChildren(TYPE) ecTable ## TYPE ## GetChildren

#define ECTABLE_OBJ_DECL(TYPE)\
ObjectID ecTableAdd(TYPE)(ECTable* table, TYPE* original, ObjectID parent);\
bool ecTableRemove(TYPE)(ECTable* table, ObjectID id);\
ObjectID ecTableGetComponent(TYPE)(const ECTable* table, ObjectID entity);\
uint32_t ecTableGetChildren(TYPE)(const ECTable* table, ObjectID entity, ObjectID* outEntities, uint32_t maxOut);

#ifdef __cplusplus
};
#endif //__cplusplus


#endif //ECTABLE_H
