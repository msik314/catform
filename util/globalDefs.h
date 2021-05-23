#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

#ifdef SHARED

#include "api/api.h"

#define CAT_MALLOC(size) catMalloc(size)
#define CAT_MEMALIGN(alignment, size) catMemalign(alignment, size)
#define CAT_REALLOC(ptr, size) catRealloc(ptr, size)
#define CAT_FREE(ptr) catFree(ptr)

#endif //SHARED

#endif //GLOBAL_DEFS_H
