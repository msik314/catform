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

typedef struct
{
    uint32_t numDependencies;
    uint32_t dependencies[];
}
JobDependency;

typedef struct _ECSystem
{
    SysInitFun sysInit;
    SysDestroyFun sysDestroy;
    
    SysColCreateFun colCreate;
    
    SysReadyFun compReady;
    const JobDependency* readyDeps;
    
    SysUpdateFun sysUpdate;
    SysCopyFun compCopy;
    const JobDependency* copyDeps;
    
    SysComponentDestroyFun compDestroy;
    const JobDependency* destroyDeps;
    
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
