#ifndef CVEC_H
#define CVEC_H

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    float x;
    float y;
}
Vec2;

typedef struct
{
    float x;
    float y;
    float z;
}
Vec3;

typedef struct
{
    float x;
    float y;
    float z;
    float w;
}
Vec4;

typedef Vec4 Quat;

#define QUAT_IDENTITY ((Quat){0, 0, 0, 1})

typedef union
{
    float data[16];
    float matrix[4][4];
    Vec4 columns[4];
}
Mat4;

#define MAT4_IDENTITY ((Mat4){1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1})

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //CVEC_H
