#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#else //__cplusplus
#include <stdbool.h>
#endif //__cplusplus

static void printErr(int32_t errNo, const char* description)
{
    if(description)
    {
        fprintf(stderr, "Error %x: %s\n", errNo, description);
    }
    else
    {
        fprintf(stderr, "Error %x\n",errNo);
    }
}

static void printWarn(int32_t errNo, const char* description)
{
    if(description)
    {
        fprintf(stderr, "Warning %x: %s\n", errNo, description);
    }
    else
    {
        fprintf(stderr, "Warning %x\n",errNo);
    }
}

static void printMsg(const char* msg)
{
    puts(msg);
}

#define CAT_SUCCESS 0
#define CAT_MSG 0x80000000

#define CAT_ERROR_JSON -1
#define CAT_ERROR_VK_INSTANCE -2
#define CAT_ERROR_VK_VALIDATION -3
#define CAT_ERROR_VK_PHYS_DEVICE -4
#define CAT_ERROR_VK_QUEUE_FAMILY -5
#define CAT_ERROR_VK_DEVICE -6
#define CAT_ERROR_WINDOW -7
#define CAT_ERROR_PIPELINE -8
#define CAT_ERROR_FRAMEBUFFER -9
#define CAT_ERROR_VK_CMD_POOL -10
#define CAT_ERROR_VK_CMD_BUFFER -11
#define CAT_ERROR_VK_SYNC -12
#define CAT_ERROR_VK_COMMAND -13
#define CAT_ERROR_PRESENT -14
#define CAT_ERROR_VRAM -15
#define CAT_ERROR_STAGING_MEMORY -16
#define CAT_ERROR_UNIFORMS -17
#define CAT_ERROR_TEXTURE -18
#define CAT_ERROR_MESH -19
#define CAT_ERROR_CLI -20
#define CAT_ERROR_FILE -21

#define CAT_WARNING_VK_INSTANCE 2
#define CAT_WARNING_VK_VALIDATION 3
#define CAT_WARNING_VK_PHYS_DEVICE 4
#define CAT_WARNING_MINIMIZED 7
#define CAT_WARNING_PRESENT 14
#define CAT_WARNING_VRAM 15
#define CAT_WARNING_TEXTURE 18
#define CAT_WARNING_CLI 20
#define CAT_WARNING_FILE 21
#define CAT_WARNING_TEX_BANK 22
#define CAT_WARNING_EC_SYSTEM 23

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //ERROR_H
