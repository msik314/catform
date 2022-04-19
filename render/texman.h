#ifndef TEXMAN_H
#define TEXMAN_H

#include <stdint.h>

#include "core/tag.h"
#include "containers/ring.h"
#include "containers/hashmap.h"
#include "render/texture.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define CAT_MAX_LOADED_TEXTURE_BANKS 8
#define CAT_INVALID_TEXTURE 0xffffffff
#define CAT_MIN_TEXTURE_SLOT 4

typedef struct
{
    TextureBank bank;
    Tag* tagList;
    uint32_t opCounter;
}
TextureSlot;

RING_DECL(TextureSlot)

static inline uint32_t textureSlotOp(TextureSlot* textureSlot) {return textureSlot->opCounter & 0xfffffffe;}
static inline bool textureSlotFree(TextureSlot* textureSlot) {return textureSlot->opCounter & 0x00000001;}

static inline void textureSlotSetOp(TextureSlot* textureSlot, uint32_t opCounter)
{
    textureSlot->opCounter = textureSlotFree(textureSlot) | (opCounter & 0xfffffffe);
}

static inline void textureSlotSetFree(TextureSlot* textureSlot, bool free)
{
    textureSlot->opCounter = textureSlotOp(textureSlot) | (free != false);
}

typedef struct
{
    TextureSlot loadedBanks[CAT_MAX_LOADED_TEXTURE_BANKS];
    
    Hashmap(Tag, Texture) loadedTextures;
    
    TextureBank fallback;
    
    Ring(TextureSlot) garbage;
    uint32_t opCounter;
    uint32_t maxSlot;
}
TexMan;

int32_t texManCreate(TexMan* texMan);
void texManDestroy(TexMan* texMan);

Texture texManTryLoadWeak(TexMan* texMan, Tag name, const void* data, uint32_t width, uint32_t height);
Texture texManTryLoadStrong(TexMan* texMan, Tag name, const void* data, uint32_t width, uint32_t height);
Texture texManLoad(TexMan* texMan, Tag name, const void* data, uint32_t width, uint32_t height);

void texManFree(TexMan* texMan, Texture tex);
void texManRealloc(TexMan* texMan, Texture tex);
void texManClean(TexMan* texMan);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //TEXMAN_H
