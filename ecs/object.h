#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define OBJECT_FLAG_REMOVE 0x00000001
#define OBJECT_FLAG_UNREADY 0x00000002
    
typedef uint32_t ObjectID;

typedef struct
{
    ObjectID id;
    volatile uint32_t flags;
    ObjectID parent;
}
Object;

#define INVALID_OBJECT 0xffffffff

#ifdef __cplusplus
};
#endif //__cplusplus


#endif //OBJECT_H
