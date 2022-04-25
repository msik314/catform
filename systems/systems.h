#ifndef SYSTEMS_H
#define SYSTEMS_H

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define SYSTEM(TYPE) SYSTEM_ ## TYPE

typedef enum SystemType
{
    SYSTEM(Entity),
    SYSTEM(Render),
    SYSTEM(CameraComponent),
    SYSTEM(SpriteComponent),
    
    NUM_SYSTEMS
}
SystemType;

#define MAKE_JOB_ID(sysOrCol, phase) ((sysOrCol) + (phase) * NUM_SYSTEMS)

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SYSTEMS_H
