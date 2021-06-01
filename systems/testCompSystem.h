#ifndef TEST_COMP_SYSTEM_H
#define TEST_COMP_SYSTEM_H

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "components/testComp.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

void testCompSysInit(ECSystem* self);
void testCompSysDestroy(ECSystem* self);

void testCompCompReady(ECSystem* self, ECTColumn* column);
void testCompSysFlags(ECSystem* self, void** flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime);
void testCompCompUpdate(ECSystem* self, ECTColumn* column, const void** flags, uint32_t numFlags, float deltaTime);
void testCompCompDestroy(ECSystem* self, ECTColumn* column);

void testCompCompReadyAll(ECSystem* self, ECTColumn* column);
void testCompCompDestroyAll(ECSystem* self, ECTColumn* column);

void testCompReady(TestComp* testComp);
void testCompOnDestroy(TestComp* testComp);

extern const ECSystem TEST_COMP_SYSTEM;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //TEST_COMP_SYSTEM_H
