#include "util/globalDefs.h"
#include "util/resourceMap.h"

#include <string.h>
#include <stdio.h>
#include <stb/stb_image.h>

#include "core/tag.h"
#include "containers/hashmap.h"
#include "containers/string.h"
#include "render/texture.h"
#include "render/texman.h"
#include "ecs/tableCache.h"
#include "ecs/ecTable.h"
#include "json/jsonData.h"
#include "json/jsonReader.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

ResourceMap* resourceMapGetInstance()
{
    static ResourceMap map;
    return &map;
}

int32_t resourceMapFromJson(ResourceMap* resourceMap, const JsonData* data, const JsonObject* resources)
{
    HashmapIterator(Tag, ValPath) mapItr;
    TexMan* texMan = texManGetInstance();
    TableCache* tableCache = tableCacheGetInstance();
    Tag key;
    
    mapItr = hashmapBegin(Tag, ValPath)(resourceMap);
    while(!hashmapIteratorEnd(Tag, ValPath)(&mapItr))
    {
        hashmapIteratorKey(Tag, ValPath)(&mapItr, &key);
        texManFreeTag(texMan, key);
        tableCacheFree(tableCache, key);
        hashmapIteratorNext(Tag, ValPath)(&mapItr);
    }
    
    hashmapClear(Tag, ValPath)(resourceMap);
    
    resourceMapAddFromJson(resourceMap, data, resources);
}

int32_t resourceMapAddFromJson(ResourceMap* resourceMap, const JsonData* data, const JsonObject* resources)
{
    HashmapIterator(Tag, JsonValue) jsonItr;
    TexMan* texMan = texManGetInstance();
    Tag key;
    JsonValue jsonVal;
    ValPath valPath;
    
    jsonItr = hashmapBegin(Tag, JsonValue)(&resources->object);
    while(!hashmapIteratorEnd(Tag, JsonValue)(&jsonItr))
    {
        hashmapIteratorKey(Tag, JsonValue)(&jsonItr, &key);
        hashmapIteratorValue(Tag, JsonValue)(&jsonItr, &jsonVal);
        
        strncpy(valPath.path, jsonVal.stringValue, sizeof(ValPath));
        valPath.path[sizeof(ValPath) - 1] = '\0';
        texManReallocTag(texMan, key);
        
        hashmapSet(Tag, ValPath)(resourceMap, &key, &valPath);
        
        hashmapIteratorNext(Tag, JsonValue)(&jsonItr);
    }
}

Texture resourceMapLoadTexture(const ResourceMap* resourceMap, Tag texName)
{
    Texture res;
    TexMan* texMan = texManGetInstance();
    uint8_t* data;
    ValPath path;
    uint32_t width, height, numChannels;
    
    res = texManGetTexture(texMan, texName);
    if(res != CAT_INVALID_TEXTURE) return res;
    
    if(!hashmapGet(Tag, ValPath)(resourceMap, &texName, &path)) return CAT_INVALID_TEXTURE;
    data = stbi_load(path.path, &width, &height, &numChannels, 4); //Force RGBA
    
    if(!data) return CAT_INVALID_TEXTURE;
    
    res = texManLoad(texMan, texName, data, width, height);
    stbi_image_free(data);
    return res;
}

ECTable* resourceMapLoadTable(ResourceMap* resourceMap, Tag tableName)
{
    ECTable* res;
    TableCache* tableCache = tableCacheGetInstance();
    JsonData data = {};
    ValPath path;
    uint32_t idx;
    const JsonObject* resources;
    
    res = tableCacheGet(tableCache, tableName);
    
    if(res) return res;
    
    if(!hashmapGet(Tag, ValPath)(resourceMap, &tableName, &path)) return NULL;
    
    jsonLoadf(&data, path.path);
    
    jsonObjectGetKey(&data.root, "resources", &idx);
    resources = jsonDataGetChildConst(&data, idx);
    resourceMapAddFromJson(resourceMap, &data, resources);
    jsonObjectGetKey(&data.root, "table", &idx);
    res = tableCacheLoad(tableCache, tableName, &data, idx);
    
    jsonDataDestroy(&data);
    return res;
}
