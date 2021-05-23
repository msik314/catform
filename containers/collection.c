#include "util/globalDefs.h"
#include "containers/collection.h"

#include <stdint.h>

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

void collectionDestroyGeneric(Collection* collection)
{
    if(collection->data) CAT_FREE(collection->data);
}

#define COLLECTION_IMPL(TYPE)\
\
void collectionCreate(TYPE)(Collection(TYPE)* collection)\
{\
    collection->data = NULL;\
    collection->size = 0;\
    collection->capacity = 0;\
}\
\
void collectionDestroy(TYPE)(Collection(TYPE)* collection)\
{\
    if(collection->data) CAT_FREE(collection->data);\
}\
\
void collectionAdd(TYPE)(Collection(TYPE)* collection, const TYPE* original)\
{\
    if(collection->size == collection->capacity)\
    {\
        collection->capacity = collection->capacity + collection->capacity + (collection->capacity == 0);\
        TYPE* temp = (TYPE*)CAT_MALLOC(collection->capacity * sizeof(TYPE));\
        for(uint32_t i = 0; i < collection->size; ++i)\
        {\
            temp[i] = collection->data[i];\
        }\
        if(collection->data) CAT_FREE(collection->data);\
        collection->data = temp;\
    }\
    collection->data[collection->size++] = *original;\
}\
\
void collectionRemove(TYPE)(Collection(TYPE)* collection, uint32_t index)\
{\
    collection->data[index] = collection->data[--(collection->size)];\
}\
\
void collectionPopN(TYPE)(Collection(TYPE)* collection, uint32_t amt)\
{\
    collection->size -= amt;\
}\
\
void collectionPop(TYPE)(Collection(TYPE)* collection)\
{\
   --(collection->size);\
}\
\
void collectionClear(TYPE)(Collection(TYPE)* collection)\
{\
    CAT_FREE(collection->data);\
    collection->data = NULL;\
    collection->size = 0;\
    collection->capacity = 0;\
}

#include "containers/collectionImpls.h"
