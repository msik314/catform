#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include <GL/gl3w.h>
#include "containers/hashmap.h"
#include "core/tag.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#define CAT_MAX_TEXTURE_BANK_SIZE 128

typedef uint32_t Texture;

HASHMAP_DECL(Tag, Texture);

typedef struct
{
    GLuint texture;
    uint64_t freeMask[2];
    uint32_t width;
    uint32_t height;
    uint32_t numImages;
    uint32_t id;
}
TextureBank;

int32_t textureBankCreate(TextureBank* textureBank, uint32_t id, uint32_t width, uint32_t height, uint32_t numImages);
void textureBankDestroy(TextureBank* textureBank);
Texture textureBankAlloc(TextureBank* textureBank, const void* textureData, uint32_t dataSize);
void textureBankFree(TextureBank* textureBank, Texture texture);
void textureBankRealloc(TextureBank* textureBank, Texture texture);
bool textureBankEmpty(TextureBank* textureBank);
bool textureBankFull(TextureBank* textureBank);
void textureBankBindSlot(TextureBank* textureBank, uint32_t slot);

//Alias for textureBankAlloc 
static inline Texture textureBankTexture(TextureBank* textureBank, const void* textureData, uint32_t dataSize)
{
    return textureBankAlloc(textureBank, textureData, dataSize);
}

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //TEXTURE_H
