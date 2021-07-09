#include "util/globalDefs.h"
#include "ecs/ectVirtualTable.h"

#include <stdint.h>
#include "components/component.h"

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
