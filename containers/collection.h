#ifndef COLLECTION_H 
#define COLLECTION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    void* data;
    uint32_t size;
    uint32_t capacity;
}
Collection;

void collectionDestroyGeneric(Collection* collection);

#define Collection(TYPE) Collection ## TYPE

#define collectionCreate(TYPE) collection ## TYPE ## Create
#define collectionDestroy(TYPE) collection ## TYPE ## Destroy
#define collectionAdd(TYPE) collection ## TYPE ## Add
#define collectionRemove(TYPE) collection ## TYPE ## Remove
#define collectionPopN(TYPE) collection ## TYPE ## PopN
#define collectionPop(TYPE) collection ## TYPE ## Pop
#define collectionClear(TYPE) collection ## TYPE ## Clear

#define COLLECTION_DECL(TYPE) \
typedef struct\
{\
    TYPE* data;\
    uint32_t size;\
    uint32_t capacity;\
}\
Collection(TYPE);\
\
void collectionCreate(TYPE)(Collection(TYPE)* collection);\
void collectionDestroy(TYPE)(Collection(TYPE)* collection);\
void collectionAdd(TYPE)(Collection(TYPE)* collection, const TYPE* original);\
void collectionRemove(TYPE)(Collection(TYPE)* collection, uint32_t index);\
void collectionPopN(TYPE)(Collection(TYPE)* collection, uint32_t amt);\
void collectionPop(TYPE)(Collection(TYPE)* collection);\
void collectionClear(TYPE)(Collection(TYPE)* collection);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //COLLECTION_H
