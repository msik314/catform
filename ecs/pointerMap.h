#ifndef POINTER_MAP_H
#define POINTER_MAP_H

#include <stdint.h>
#include "util/utilMacros.h"
#include "ecs/object.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus  

typedef union _PointerMapEntry
{
    struct
    ALIGNED(8,{
        uint32_t value;
        uint32_t checkID;
    });
    union _PointerMapEntry* next;
}
PointerMapEntry;

typedef struct _PointerMap
{
    ALIGNED(16, volatile PointerMapEntry* head);
    volatile uint64_t popCounter;
    PointerMapEntry* entries;
    volatile uint32_t size;
    uint32_t capacity;
}
PointerMap;

void pointerMapCreate(PointerMap* map, uint32_t size);
void pointerMapDestroy(PointerMap* map);

ObjectID pointerMapAdd(PointerMap* map, uint32_t idx);
void pointerMapSet(PointerMap* map, ObjectID id, uint32_t idx);
uint32_t pointerMapGet(const PointerMap* map, ObjectID id);
void pointerMapRemove(PointerMap* map, ObjectID id);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //POINTER_MAP_H
