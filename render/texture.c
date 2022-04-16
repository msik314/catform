#include "util/globalDefs.h"
#include "render/texture.h"

#include <GL/gl3w.h>
#include "core/error.h"
#include "util/assert.h"
#include "util/utilMacros.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

static inline uint64_t setFree(int32_t count)
{
    if(count < 0) return 0;
    if(count > 63) return 0xffffffffffffffff;
    return (1ull << count) - 1;
}

int32_t textureBankCreate(TextureBank* textureBank, uint32_t id, uint32_t width, uint32_t height, uint32_t numImages)
{
    for(int32_t i = 0; i < 2; ++i)
    {
        textureBank->freeMask[i] = setFree((int32_t)numImages - i * 64);
    }
    
    textureBank->width = width;
    textureBank->height = height;
    textureBank->id = id;
    
    ASSERT(numImages <= CAT_MAX_TEXTURE_BANK_SIZE, "Texture bank image count exceeds the maximum supported value 128");
    textureBank->numImages = numImages;
    
    glGenTextures(1, &textureBank->texture);
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureBank->texture);  
    
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, numImages, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    return CAT_SUCCESS;
}

void textureBankDestroy(TextureBank* textureBank)
{
    glDeleteTextures(1, &textureBank->texture);
}

Texture textureBankAlloc(TextureBank* textureBank, const void* textureData, uint32_t dataSize)
{
    int32_t idx = -1;
    uint32_t mask = 0;
    uint32_t textureSize;
    uint32_t texture; 
    
    for(uint32_t i = 0; i < 4; ++i)
    {
        if(textureBank->freeMask[i] == 0) continue;
        
        idx = firstSet(textureBank->freeMask[i]);
        mask = ~(1 << idx);
        textureBank->freeMask[i] &= mask;
        
        idx += i * 64;
        break;
    }
    
    if(idx == -1) return 0xffffffff;
    
    glActiveTexture(GL_TEXTURE0 + textureBank->id);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, idx, textureBank->width, textureBank->height, 1, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    
    texture = textureBank->id;
    texture <<= 16;
    texture |= idx;
    return texture;
}

void textureBankFree(TextureBank* textureBank, Texture texture)
{
    uint32_t idx = texture & 0x0000003f;
    uint32_t mask = (texture >> 6) & 0x00000003;
    
    textureBank->freeMask[mask] |= (1 << idx);
}

bool textureBankEmpty(TextureBank* textureBank)
{
    for(int32_t i = 0; i < 4; ++i)
    {
        if(textureBank->freeMask[i] != setFree((int32_t)(textureBank->numImages) - i * 64)) return false;
    }
    
    return true;
}

bool textureBankFull(TextureBank* textureBank)
{
    for(int32_t i = 0; i < 4; ++i)
    {
        if(textureBank->freeMask[i] != 0) return false;
    }
    
    return true;
}
