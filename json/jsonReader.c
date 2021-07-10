#include "util/globalDefs.h"
#include "json/jsonReader.h"
#include "json/jsonData.h"

#include <stdio.h>

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

void jsonLoadf(JsonData* data, const char* fileName)
{
    char* str;
    int32_t len;
    FILE* file = fopen(fileName, "r");
    
    fseek(file, 0, SEEK_END);
    len = ftell(file);
    str = (char*)CAT_MALLOC(len * sizeof(char));
    rewind(file);
    fread(str, sizeof(char), len, file);
    fclose(file);
    
    jsonLoads(data, str, len);
    CAT_FREE(str);
}
