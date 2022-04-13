#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <cmath/cvec.h>

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    Quat rotation;
    Vec3 position;
    Vec3 scale;
}
Transform;

void transformCompose(const Transform* transform, Mat4* outMatrix);
bool transformDecompose(const Mat4* matrix, Transform* outTransform);

#define TRANSFORM_IDENTITY ((Transform){QUAT_IDENTITY, {0, 0, 0}, {1, 1, 1}})

#ifdef __cplusplus
};
#endif //__cplusplus  

#endif //TRANSFORM_H
