#include "util/globalDefs.h"
#include "systems/cameraSystem.h"

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/sceneManager.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "ecs/phase.h"
#include "core/tag.h"
#include "components/cameraComponent.h"
#include "systems/systems.h"
#include "util/atomics.h"

const JobDependency CAMERA_READY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(CameraComponent), PHASE_PARENT)}};
const JobDependency CAMERA_COPY_DEPS = {1, {MAKE_JOB_ID(SYSTEM(CameraComponent), PHASE_UPDATE)}};

const JobDependency CAMERA_DESTROY_DEPS = 
{
    2, 
    {MAKE_JOB_ID(COMPONENT(CameraComponent), PHASE_MARK), MAKE_JOB_ID(SYSTEM(Render), PHASE_UPDATE)}
};

static void cameraColCreate(ECTColumn* column){ectColumnCreate(CameraComponent)((ECTColumn(CameraComponent)*)column);}

const ECSystem CAMERA_SYSTEM =
{
    cameraSysInit,
    cameraSysDestroy,
    
    cameraColCreate,
    
    cameraCompReady, &CAMERA_READY_DEPS,
    cameraSysUpdate,
    cameraCompCopy, &CAMERA_COPY_DEPS,
    cameraCompDestroy, &CAMERA_DESTROY_DEPS,
    
    cameraCompReadyAll,
    cameraCompDestroyAll
};

void cameraSysInit(ECSystem* self){}
void cameraSysDestroy(ECSystem* self){}

void cameraCompReady(ECSystem* self, ECTColumn* column)
{
    ECTColumn(CameraComponent)* cameras = (ECTColumn(CameraComponent)*)column;
    uint32_t idx = cameras->components.size;
    uint32_t flags;
    
    while(idx)
    {
        --idx;
        
        flags = atomicLoad32(&cameras->components.data[idx].self.flags);
        if(!(flags & OBJECT_FLAG_UNREADY))
        {
            return;
        }
        
        fetchAnd32(&cameras->components.data[idx].self.flags, ~OBJECT_FLAG_UNREADY);
        cameraReady(&cameras->components.data[idx]);
    }
}

void cameraSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime){}

void cameraCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime){}

void cameraCompDestroy(ECSystem* self, ECTColumn* column)
{
    ECTColumn(CameraComponent)* cameras = (ECTColumn(CameraComponent)*)column;
    uint32_t flags;
    
    for(uint32_t i = 0; i < cameras->components.size; ++i)
    {
        flags = atomicLoad32(&cameras->components.data[i].self.flags);
        if(flags & OBJECT_FLAG_REMOVE) cameraOnDestroy(&cameras->components.data[i]);
    }
}

void cameraCompReadyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(CameraComponent)* cameras = (ECTColumn(CameraComponent)*)column;
    
    for(uint32_t i = 0; i < cameras->components.size; ++i)
    {
        fetchAnd32(&cameras->components.data[i].self.flags, ~OBJECT_FLAG_UNREADY);
        cameraReady(&cameras->components.data[i]);
    }
}

void cameraCompDestroyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(CameraComponent)* cameras = (ECTColumn(CameraComponent)*)column;
    CameraComponent cam;
    
    for(uint32_t i = 0; i < cameras->components.size; ++i)
    {
        cameraOnDestroy(&cameras->components.data[i]);
    }
    
    while(mwQueuePop(CameraComponent)(&cameras->addQueue, &cam))
    {
        cameraOnDestroy(&cam);
    }
}

void cameraReady(CameraComponent* camera){}
void cameraOnDestroy(CameraComponent* camera){} 
