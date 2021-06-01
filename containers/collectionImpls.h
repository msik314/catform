#include "containers/collectionTypes.h"
COLLECTION_IMPL(uint8_t)
COLLECTION_IMPL(uint16_t)
COLLECTION_IMPL(uint32_t)
COLLECTION_IMPL(uint64_t)
COLLECTION_IMPL(int8_t)
COLLECTION_IMPL(int16_t)
COLLECTION_IMPL(int32_t)
COLLECTION_IMPL(int64_t)
COLLECTION_IMPL(char)
COLLECTION_IMPL(bool)
COLLECTION_IMPL(float)
COLLECTION_IMPL(double)

#include "components/entity.h"
COLLECTION_IMPL(Entity);

#include "components/testComp.h"
COLLECTION_IMPL(TestComp);
