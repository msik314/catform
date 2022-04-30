#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "json/jsonData.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

int32_t configLoadInput(const JsonData* config);
int32_t configSaveInput(JsonData* config);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //CONFIG_H
