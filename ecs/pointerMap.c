#include "util/globalDefs.h"
#include "ecs/pointerMap.h"

#include "util/atomics.h"
#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

static inline uint32_t toIndex(ObjectID id) {return id & 0x0fffffff;}

typedef struct ALIGNED(16,
{
    PointerMapEntry* head;
    uint64_t popCounter;
})
FreeListEntry;

void pointerMapCreate(PointerMap* map, uint32_t size)
{
    atomicStore64(&map->head, NULL);
    atomicStore64(&map->popCounter, 0);
    
    map->entries = CAT_MALLOC(size * sizeof(PointerMapEntry));
    
    atomicStore32(&map->size, 0);
    map->capacity = size;
}

void pointerMapDestroy(PointerMap* map)
{
    CAT_FREE(map->entries);
}

ObjectID pointerMapAdd(PointerMap* map, uint32_t idx)
{
    PointerMapEntry* head;
    PointerMapEntry* next;
    uint64_t popCounter;
    FreeListEntry expected;
    uint32_t size;
    bool success = false;
    expected.head = (PointerMapEntry*)atomicLoad64(&map->head);
    expected.popCounter = atomicLoad64(&map->popCounter);
    
    while(expected.head && !success)
    {
        head = expected.head;
        next = head->next;
        popCounter = expected.popCounter + 1;
        success = compareAndSwap128(&map->head, &expected, (uint64_t)next, popCounter);
    }
    
    if(!success)
    {
        size = fetchAdd32(&map->size, 1);
        if(size >= map->capacity)
        {
            return INVALID_OBJECT;
        }
        
        head = &(map->entries[size]);
        head->checkID = size;
    }
    
    head->value = idx;
    return head->checkID;
}

void pointerMapSet(PointerMap* map, ObjectID id, uint32_t idx)
{
    map->entries[toIndex(id)].value = idx;
}

uint32_t pointerMapGet(const PointerMap* map, ObjectID id)
{
    PointerMapEntry e = map->entries[toIndex(id)];
    return e.checkID == id ? e.value : INVALID_OBJECT;
}

void pointerMapRemove(PointerMap* map, ObjectID id)
{
    PointerMapEntry* expected = (PointerMapEntry*)atomicLoad64(&map->head);
    PointerMapEntry* entry = &(map->entries[toIndex(id)]);
    entry->checkID += 0x10000000;
    do
    {
        entry->next = expected;
    }
    while(!compareAndSwap64(&map->head, &expected, entry));
}
