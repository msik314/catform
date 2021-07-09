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

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //ECT_VIRTUAL_TABLE_H
