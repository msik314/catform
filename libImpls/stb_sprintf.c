#include "util/globalDefs.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

#define STB_SPRINTF_IMPLEMENTATION
#include <stb/stb_sprintf.h>
