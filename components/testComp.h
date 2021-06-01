#ifndef TEST_COMP_H
#define TEST_COMP_H

#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "util/atomics.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    Object self;
    uint32_t number;
}
TestComp;

COLLECTION_DECL(TestComp)
MWQUEUE_DECL(TestComp)
ECTCOLUMN_DECL(TestComp)

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //TEST_COMP_H
