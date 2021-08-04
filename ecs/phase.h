#ifndef PHASE_H
#define PHASE_H

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define SYSTEM(TYPE) SYSTEM_ ## TYPE

typedef enum Phase
{
    PHASE_UPDATE,
    PHASE_COPY,
    PHASE_MARK,
    PHASE_DESTROY,
    PHASE_AR,
    PHASE_PARENT,
    PHASE_READY,
    
    NUM_PHASES,
    
    PHASE_INVALID = -1
}
Phase;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //PHASE_H
