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
    
    NUM_COMPONENT_TYPES
}
ComponentType;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //COMPONENT_TYPES_H
