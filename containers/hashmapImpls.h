#include "containers/hashmapTypes.h"

#include "core/tag.h"
#include "json/jsonData.h"
HASHMAP_IMPL_EQ(Tag, JsonValue, tagEq)

#include "ecs/object.h"
HASHMAP_IMPL(ObjectID, ObjectID)

#include "render/texture.h"
HASHMAP_IMPL_EQ(Tag, Texture, tagEq)

#include "util/resourceMap.h"
HASHMAP_IMPL_EQ(Tag, ValPath, tagEq)
