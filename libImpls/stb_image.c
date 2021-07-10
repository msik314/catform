#include "util/globalDefs.h"

#include "util/assert.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_REALLOC(PTR, SIZE) realloc(PTR, SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC


#define STBI_ASSERT(X) ASSERT(X, "")
#define STBI_MALLOC(size) CAT_MALLOC(size)  
#define STBI_REALLOC(ptr, size) CAT_REALLOC(ptr, size)
#define STBI_FREE(ptr) CAT_FREE(ptr)


#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
