#ifndef LEAK_CHECK_H
#define LEAK_CHECK_H

#ifndef NDEBUG

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#include "containers/hashmap.h"
    
typedef struct
{
    const char* fileName;
    uint32_t line;
}
AllocData;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //NDEBUG

#endif //LEAK_CHECK_H
