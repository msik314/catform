#ifndef COMPONENT_TYPES_H
#define COMPONENT_TYPES_H

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define COMPONENT(TYPE) COMPONENT_ ## TYPE

typedef enum ComponentTypes
{
    COMPONENT(Entity),
    COMPONENT(PlayerComponent),
    COMPONENT(CameraComponent),
    COMPONENT(SpriteComponent),
    
    NUM_COMPONENT_TYPES
}
ComponentType;

#define getComponents(columns, type) ((type*)((columns)[COMPONENT(type)].components.data))
#define getComponentsConst(column, type) ((const type*)((columns)[COMPONENT(type)].components.data))
#define getNumComponents(columns, type) ((columns)[COMPONENT(type)].components.size)

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //COMPONENT_TYPES_H
