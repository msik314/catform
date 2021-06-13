#ifndef ECSYSTEM_H
#define ECSYSTEM_H

#include "ecs/ectColumn.h"
#include "ecs/pointerMap.h"
#include "containers/vector.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef void* volatile SysFlags;

struct _ECSystem;

typedef void (*SysInitFun)(struct _ECSystem* self);
typedef void (*SysDestroyFun)(struct _ECSystem* self);

typedef void (*SysColCreateFun)(ECTColumn* column);

typedef void (*SysReadyFun)(struct _ECSystem* self, ECTColumn* column);
typedef void (*SysUpdateFun)(struct _ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime);
typedef void (*SysCopyFun)(struct _ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime);
typedef void (*SysComponentDestroyFun)(struct _ECSystem* self, ECTColumn* column);

typedef struct _ECSystem
{
    SysInitFun sysInit;
    SysDestroyFun sysDestroy;
    
    SysColCreateFun colCreate;
    
    SysReadyFun compReady;
    SysUpdateFun sysUpdate;
    SysCopyFun compCopy;
    SysComponentDestroyFun compDestroy;
    
    SysReadyFun compReadyAll;
    SysComponentDestroyFun compDestroyAll;
    
    void* state;
}
ECSystem;
VECTOR_DECL(ECSystem);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //ECSYSTEM_H
