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
#include "json/jsonData.h"

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
    HashmapIterator(Tag, JsonValue) jsonItr;
    TexMan* texMan = texManGetInstance();
    Tag key;
    JsonValue jsonVal;
    ValPath valPath;
    const JsonObject* pathName;
    
    mapItr = hashmapBegin(Tag, ValPath)(resourceMap);
    while(!hashmapIteratorEnd(Tag, ValPath)(&mapItr))
    {
        hashmapIteratorKey(Tag, ValPath)(&mapItr, &key);
        texManFreeTag(texMan, key);
        hashmapIteratorNext(Tag, ValPath)(&mapItr);
    }
    
    hashmapClear(Tag, ValPath)(resourceMap);
    
    jsonItr = hashmapBegin(Tag, JsonValue)(&resources->object);
    while(!hashmapIteratorEnd(Tag, JsonValue)(&jsonItr))
    {
        hashmapIteratorKey(Tag, JsonValue)(&jsonItr, &key);
        hashmapIteratorValue(Tag, JsonValue)(&jsonItr, &jsonVal);
        
        pathName = jsonDataGetChildConst(data, jsonVal.objectIndex);
        strncpy(valPath.path, pathName->string.data, sizeof(ValPath));
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
