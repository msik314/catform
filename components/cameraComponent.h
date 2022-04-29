#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "containers/collection.h"
#include "containers/mwQueue.h"
#include "ecs/object.h"
#include "ecs/ectColumn.h"
#include "cmath/cvec.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    Object self;
    Mat4 projection;
}
CameraComponent;

COLLECTION_DECL(CameraComponent)
MWQUEUE_DECL(CameraComponent)
ECTCOLUMN_DECL(CameraComponent)

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //CAMERA_COMPONENT_H
