#ifndef ECT_VIRTUAL_TABLE_H
#define ECT_VIRTUAL_TABLE_H

#include <stdint.h>
#include "ecs/ectColumn.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

ECTColumnAddRemoveFun getVirtualAddRemove(uint32_t column);
void setVirtualAddRemove(uint32_t column, ECTColumnAddRemoveFun addRemove);

ECTColumnAddRemoveFun getVirtualRemoveAll(uint32_t column);
void setVirtualRemoveAll(uint32_t column, ECTColumnAddRemoveFun removeAll);

ECTColumnParentFun getVirtualParentDelete(uint32_t column);
void setVirtualParentDelete(uint32_t column, ECTColumnParentFun parentDelete);

ECTColumnParentFun getVirtualParentAdd(uint32_t column);
void setVirtualParentAdd(uint32_t column, ECTColumnParentFun parentAdd);

ECTColumnSerializeFun getVirtualSerialize(uint32_t column);
void setVirtualSerialize(uint32_t column, ECTColumnSerializeFun serialize);

ECTColumnDeserializeFun getVirtualDeserialize(uint32_t column);
void setVirtualDeserialize(uint32_t column, ECTColumnDeserializeFun deserialize);

ECTColumnGetIDsFun getVirtualGetIDs(uint32_t column);
void setVirtualGetIDs(uint32_t column, ECTColumnGetIDsFun getIds);

ECTColumnAddAllFun getVirtualAddAll(uint32_t column);
void setVirtualAddAll(uint32_t column, ECTColumnAddAllFun addAll);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //ECT_VIRTUAL_TABLE_H
