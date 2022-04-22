#include "util/globalDefs.h"
#include "render/texman.h"

#include <string.h>
#include <stdbool.h>

#include "core/error.h"
#include "core/tag.h"
#include "render/texture.h"
#include "containers/hashmap.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

#define TEXTURE_BANK_DESTROY_DELAY 4
#define MAGENTA 1, 0, 1, 1
#define BLACK 0, 0, 0, 1

const char FALLBACK_IMG[16] = {MAGENTA, BLACK, BLACK, MAGENTA};

TexMan* texManGetInstance()
{
    static TexMan texMan;
    return &texMan;
}

int32_t texManCreate(TexMan* texMan)
{
    texMan->maxSlot = 0;
    
    texMan->loadedBanks[0].opCounter = 0;
    textureSlotSetFree(&texMan->loadedBanks[0], true);
    texMan->loadedBanks[0].tagList = (Tag*)CAT_MALLOC(CAT_MAX_LOADED_TEXTURE_BANKS * CAT_MAX_TEXTURE_BANK_SIZE * sizeof(Tag));
    memset(texMan->loadedBanks[0].tagList, 0, CAT_MAX_LOADED_TEXTURE_BANKS * CAT_MAX_TEXTURE_BANK_SIZE * sizeof(Tag));
    
    for(uint32_t i = 1; i < CAT_MAX_LOADED_TEXTURE_BANKS; ++i)
    {
        texMan->loadedBanks[i].opCounter = 0;
        textureSlotSetFree(&texMan->loadedBanks[i], true);
        texMan->loadedBanks[i].tagList = &(texMan->loadedBanks[0].tagList[i * CAT_MAX_TEXTURE_BANK_SIZE]);
    }
    
    texMan->opCounter = 0;
    
    ringCreate(TextureSlot)(&texMan->garbage);
    hashmapCreate(Tag, Texture)(&texMan->loadedTextures);
    
    textureBankCreate(&texMan->fallback, 0, 2, 2, 1);
    textureBankAlloc(&texMan->fallback, (void*)FALLBACK_IMG, sizeof(FALLBACK_IMG));
    for(uint32_t i = 1; i < CAT_MAX_LOADED_TEXTURE_BANKS; ++i)
    {
        textureBankBindSlot(&texMan->fallback, CAT_MIN_TEXTURE_SLOT + i);
    }
}

void texManDestroy(TexMan* texMan)
{
    for(uint32_t i = 0; i < texMan->maxSlot; ++i)
    {
        textureBankDestroy(&texMan->loadedBanks[i].bank);
    }
    
    texMan->opCounter += (TEXTURE_BANK_DESTROY_DELAY << 1) + 2;
    texManClean(texMan);
    
    if(texMan->maxSlot <= CAT_MAX_LOADED_TEXTURE_BANKS + 2 * TEXTURE_BANK_DESTROY_DELAY)
    {
        textureBankDestroy(&texMan->fallback);
    }
    
    CAT_FREE(texMan->loadedBanks[0].tagList);
    ringDestroy(TextureSlot)(&texMan->garbage);
    hashmapDestroy(Tag, Texture)(&texMan->loadedTextures);
}

Texture texManTryLoadWeak(TexMan* texMan, Tag name, const void* data, uint32_t width, uint32_t height)
{
    Texture tex;
    uint32_t slotIdx = 0xffffffff;
    uint32_t texIdx;
    
    for(uint32_t i = 0; i < CAT_MAX_LOADED_TEXTURE_BANKS; ++i)
    {
        if(texMan->loadedBanks[i].bank.width != width ||
            texMan->loadedBanks[i].bank.height != height ||
            textureBankFull(&texMan->loadedBanks[i].bank))
        {
            continue;
        }
        slotIdx = i;
        break;
    }
    
    if(slotIdx == 0xffffffff) return CAT_INVALID_TEXTURE;
    
    textureSlotSetOp(&texMan->loadedBanks[slotIdx], texMan->opCounter);
    tex = textureBankAlloc(&texMan->loadedBanks[slotIdx].bank, data, width * height);
    
    if(tex != CAT_INVALID_TEXTURE)
    {
        texIdx = (uint32_t)tex & 0x000000ff;
        if(!tagEmpty(&texMan->loadedBanks[slotIdx].tagList[texIdx]))
        {
            hashmapRemove(Tag, Texture)(&texMan->loadedTextures, &texMan->loadedBanks[slotIdx].tagList[texIdx]);
        }
        
        tagSet(&texMan->loadedBanks[slotIdx].tagList[texIdx], name.data);
        textureSlotSetFree(&texMan->loadedBanks[slotIdx], false);
    }
    
    hashmapSet(Tag, Texture)(&texMan->loadedTextures, &name, &tex);
    return tex;
}

Texture texManTryLoadStrong(TexMan* texMan, Tag name, const void* data, uint32_t width, uint32_t height)
{
    Texture tex;
    uint32_t maxOpDiff = 0;
    uint32_t opDiff = 0;
    uint32_t slotIdx = 0xffffffff;
    uint32_t texIdx;
    
    tex = texManTryLoadWeak(texMan, name, data, width, height);
    if(tex != CAT_INVALID_TEXTURE) return tex;
    
    if(texMan->maxSlot < CAT_MAX_LOADED_TEXTURE_BANKS)
    {
        slotIdx = texMan->maxSlot++;
    }
    else
    {
        return CAT_INVALID_TEXTURE;
    }
    
    textureSlotSetOp(&texMan->loadedBanks[slotIdx], texMan->opCounter);
    
    if(textureBankCreate(&texMan->loadedBanks[slotIdx].bank, slotIdx, width, height, CAT_MAX_TEXTURE_BANK_SIZE) < CAT_SUCCESS)
    {
        return CAT_INVALID_TEXTURE;
    }
    
    textureBankBindSlot(&texMan->fallback, CAT_MIN_TEXTURE_SLOT + slotIdx);
    tex = textureBankAlloc(&texMan->loadedBanks[slotIdx].bank, data, width * height);
    
    if(tex != CAT_INVALID_TEXTURE)
    {
        texIdx = (uint32_t)tex & 0x000000ff;
        tagSet(&texMan->loadedBanks[slotIdx].tagList[texIdx], name.data);
        textureSlotSetFree(&texMan->loadedBanks[slotIdx], false);
    }
    
    hashmapSet(Tag, Texture)(&texMan->loadedTextures, &name, &tex);
    return tex;
}

Texture texManLoad(TexMan* texMan, Tag name, const void* data, uint32_t width, uint32_t height)
{
    Texture tex;
    uint32_t maxOpDiff = 0;
    uint32_t opDiff = 0;
    uint32_t slotIdx = 0xffffffff;
    uint32_t texIdx;
    bool isNewSlot = false;
    
    tex = texManTryLoadWeak(texMan, name, data, width, height);
    if(tex != CAT_INVALID_TEXTURE) return tex;
    
    if(texMan->maxSlot < CAT_MAX_LOADED_TEXTURE_BANKS)
    {
        slotIdx = texMan->maxSlot++;
        isNewSlot = true;
    }
    else
    {
        for(uint32_t i = 0; i < texMan->maxSlot; ++i)
        {
            if(!textureSlotFree(&texMan->loadedBanks[i])) continue;
            opDiff = texMan->opCounter + 1 - textureSlotOp(&texMan->loadedBanks[i]);
            if(opDiff > maxOpDiff)
            {
                maxOpDiff = opDiff;
                slotIdx = i;
            }
        }
        return CAT_INVALID_TEXTURE;
    }
    
    if(slotIdx == 0xffffffff) return CAT_INVALID_TEXTURE;
    
    textureSlotSetOp(&texMan->loadedBanks[slotIdx], texMan->opCounter);
    if(!isNewSlot)
    {
        for(uint32_t i = 0; i < CAT_MAX_TEXTURE_BANK_SIZE; ++i)
        {
            if(tagEmpty(&texMan->loadedBanks[slotIdx].tagList[i])) break;
            hashmapRemove(Tag, Texture)(&texMan->loadedTextures, &texMan->loadedBanks[slotIdx].tagList[i]);
            tagSet(&texMan->loadedBanks[slotIdx].tagList[i], "");
        }
        
        ringAdd(TextureSlot)(&texMan->garbage, &texMan->loadedBanks[slotIdx]);
    }
    
    if(textureBankCreate(&texMan->loadedBanks[slotIdx].bank, slotIdx, width, height, CAT_MAX_TEXTURE_BANK_SIZE) < CAT_SUCCESS)
    {
        return CAT_INVALID_TEXTURE;
    }
    textureBankBindSlot(&texMan->fallback, CAT_MIN_TEXTURE_SLOT + slotIdx);
    tex = textureBankAlloc(&texMan->loadedBanks[slotIdx].bank, data, width * height);
    
    if(tex != CAT_INVALID_TEXTURE)
    {
        texIdx = (uint32_t)tex & 0x000000ff;
        tagSet(&texMan->loadedBanks[slotIdx].tagList[texIdx], name.data);
        textureSlotSetFree(&texMan->loadedBanks[slotIdx], false);
    }
    
    hashmapSet(Tag, Texture)(&texMan->loadedTextures, &name, &tex);
    return tex;
}

void texManFree(TexMan* texMan, Texture tex)
{
    uint32_t slotIdx = ((uint32_t)tex >> 16) & 0x0000ffff;
    textureSlotSetOp(&texMan->loadedBanks[slotIdx], texMan->opCounter);
    textureBankFree(&texMan->loadedBanks[slotIdx].bank, tex);
    if(textureBankEmpty(&texMan->loadedBanks[slotIdx].bank))
    {
        textureSlotSetFree(&texMan->loadedBanks[slotIdx], true);
    }
}

void texManFreeTag(TexMan* texMan, Tag texName)
{
    Texture tex;
    if(hashmapGet(Tag, Texture)(&texMan->loadedTextures, &texName, &tex))
    {
        texManFree(texMan, tex);
    }
}

void texManRealloc(TexMan* texMan, Texture tex)
{
    uint32_t slotIdx = ((uint32_t)tex >> 16) & 0x0000ffff;
    textureSlotSetOp(&texMan->loadedBanks[slotIdx], texMan->opCounter);
    textureBankRealloc(&texMan->loadedBanks[slotIdx].bank, tex);
}

void texManReallocTag(TexMan* texMan, Tag texName)
{
    Texture tex;
    if(hashmapGet(Tag, Texture)(&texMan->loadedTextures, &texName, &tex))
    {
        texManRealloc(texMan, tex);
    }
}

void texManClean(TexMan* texMan)
{
    TextureSlot slot;
    
    while(texMan->garbage.size > 0)
    {
        ringGet(TextureSlot)(&texMan->garbage, &slot);
        if(texMan->opCounter - textureSlotOp(&slot) < (TEXTURE_BANK_DESTROY_DELAY << 1)) break;
        
        ringPop(TextureSlot)(&texMan->garbage);
        textureBankDestroy(&slot.bank);
    }
    
    if(texMan->maxSlot == CAT_MAX_LOADED_TEXTURE_BANKS + 2 * TEXTURE_BANK_DESTROY_DELAY)
    {
        ++texMan->maxSlot;
        textureBankDestroy(&texMan->fallback);
    }
    else if(texMan->maxSlot >= CAT_MAX_LOADED_TEXTURE_BANKS)
    {
        ++texMan->maxSlot;
    }
    
    texMan->opCounter += 2;
}

Texture texManGetTexture(TexMan* texMan, Tag name)
{
    Texture res;
    if(!hashmapGet(Tag, Texture)(&texMan->loadedTextures, &name, &res)) return CAT_INVALID_TEXTURE;
    
    texManRealloc(texMan, res);
    return res;
}
