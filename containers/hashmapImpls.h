#include "containers/hashmapTypes.h"

#include "core/tag.h"
#include "json/jsonData.h"
HASHMAP_IMPL_EQ(Tag, JsonValue, tagEq)

#include "ecs/object.h"
HASHMAP_IMPL(ObjectID, ObjectID)
