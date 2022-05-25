#ifndef RESOURCE_MAP_H
#define RESOURCE_MAP_H

#include <stdint.h>

#include "core/tag.h"
#include "containers/hashmap.h"
#include "render/texture.h"
#include "ecs/ecTable.h"
#include "json/jsonData.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define CAT_MAX_PATH 511 - sizeof(Tag) - sizeof(uint32_t)

typedef struct 
{
    char path[CAT_MAX_PATH + 1];
}
ValPath;

HASHMAP_DECL(Tag, ValPath);

typedef Hashmap(Tag, ValPath) ResourceMap;

ResourceMap* resourceMapGetInstance();

#define resourceMapCreate(map) hashmapCreate(Tag, ValPath)(map)
#define resourceMapDestroy(map) hashmapDestroy(Tag, ValPath)(map)
#define resourceMapAdd(map, tag, path) hashmapSet(Tag, ValPath)(map, tag, path)
#define resourceMapClear(map) hashmapClear(Tag, ValPath)(map)

int32_t resourceMapFromJson(ResourceMap* resourceMap, const JsonData* data, const JsonObject* resources);
int32_t resourceMapAddFromJson(ResourceMap* resourceMap, const JsonData* data, const JsonObject* resources);

Texture resourceMapLoadTexture(const ResourceMap* resourceMap, Tag texName);

ECTable* resourceMapLoadTable(ResourceMap* resourceMap, Tag tableName);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //RESOURCE_MAP_H
