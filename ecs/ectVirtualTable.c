#include "util/globalDefs.h"
#include "ecs/ectVirtualTable.h"

#include <stdint.h>
#include "components/component.h"

//AddRemove
static inline ECTColumnAddRemoveFun* getAddRemoveTable()
{
    static ECTColumnAddRemoveFun addRemoveFuns[NUM_COMPONENT_TYPES];
    return addRemoveFuns;
}

ECTColumnAddRemoveFun getVirtualAddRemove(uint32_t column)
{
    return getAddRemoveTable()[column];
}

void setVirtualAddRemove(uint32_t column, ECTColumnAddRemoveFun addRemove)
{
    getAddRemoveTable()[column] = addRemove;
}

//RemoveAll
static inline ECTColumnAddRemoveFun* getRemoveAllTable()
{
    static ECTColumnAddRemoveFun removeAllFuns[NUM_COMPONENT_TYPES];
    return removeAllFuns;
}

ECTColumnAddRemoveFun getVirtualRemoveAll(uint32_t column)
{
    return getRemoveAllTable()[column];
}

void setVirtualRemoveAll(uint32_t column, ECTColumnAddRemoveFun removeAll)
{
    getRemoveAllTable()[column] = removeAll;
}

//ParentDelete
static inline ECTColumnParentFun* getParentDeleteTable()
{
    static ECTColumnParentFun parentDeleteFuns[NUM_COMPONENT_TYPES];
    return parentDeleteFuns;
}

ECTColumnParentFun getVirtualParentDelete(uint32_t column)
{
    return getParentDeleteTable()[column];
}

void setVirtualParentDelete(uint32_t column, ECTColumnParentFun parentDelete)
{
    getParentDeleteTable()[column] = parentDelete;
}

//ParentAdd
static inline ECTColumnParentFun* getParentAddTable()
{
    static ECTColumnParentFun parentAddFuns[NUM_COMPONENT_TYPES];
    return parentAddFuns;
}

ECTColumnParentFun getVirtualParentAdd(uint32_t column)
{
    return getParentAddTable()[column];
}

void setVirtualParentAdd(uint32_t column, ECTColumnParentFun parentAdd)
{
    getParentAddTable()[column] = parentAdd;
}

//Serialize
static inline ECTColumnSerializeFun* getSerializeTable()
{
    static ECTColumnSerializeFun serializeFuns[NUM_COMPONENT_TYPES];
    return serializeFuns;
}

ECTColumnSerializeFun getVirtualSerialize(uint32_t column)
{
    return getSerializeTable()[column];
}

void setVirtualSerialize(uint32_t column, ECTColumnSerializeFun serialize)
{
    getSerializeTable()[column] = serialize;
}

//Deserialize
static inline ECTColumnDeserializeFun* getDeserializeTable()
{
    static ECTColumnDeserializeFun deserializeFuns[NUM_COMPONENT_TYPES];
    return deserializeFuns;
}

ECTColumnDeserializeFun getVirtualDeserialize(uint32_t column)
{
    return getDeserializeTable()[column];
}

void setVirtualDeserialize(uint32_t column, ECTColumnDeserializeFun deserialize)
{
    getDeserializeTable()[column] = deserialize;
}

//GetIDs
static inline ECTColumnGetIDsFun* getGetIDsTable()
{
    static ECTColumnGetIDsFun getIdsFuns[NUM_COMPONENT_TYPES];
    return getIdsFuns;
}

ECTColumnGetIDsFun getVirtualGetIDs(uint32_t column)
{
    return getGetIDsTable()[column];
}

void setVirtualGetIDs(uint32_t column, ECTColumnGetIDsFun getIds)
{
    getGetIDsTable()[column] = getIds;
}

//AddAll
static inline ECTColumnAddAllFun* getGetAddAllTable()
{
    static ECTColumnAddAllFun addAllFuns[NUM_COMPONENT_TYPES];
    return addAllFuns;
}

ECTColumnAddAllFun getVirtualAddAll(uint32_t column)
{
    return getGetAddAllTable()[column];
}

void setVirtualAddAll(uint32_t column, ECTColumnAddAllFun addAll)
{
    getGetAddAllTable()[column] = addAll;
}
