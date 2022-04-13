#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#else //__cplusplus
#include <stdbool.h>
#endif //__cplusplus

#define printErr(errNo, description) _printErr(errno, description, __FILE__, __LINE__)
static void _printErr(int32_t errNo, const char* description, const char* file, int32_t line)
{
    if(description)
    {
        fprintf(stderr, "%s %d: Error %x: %s\n", file, line, errNo, description);
    }
    else
    {
        fprintf(stderr, "%s %d Error %x\n", file, line, errNo);
    }
}

#define printWarn(errNo, description) _printWarn(errno, description, __FILE__, __LINE__)
static void _printWarn(int32_t errNo, const char* description, const char* file, int32_t line)
{
    if(description)
    {
        fprintf(stderr, "%s %d: Warning %x: %s\n", file, line, errNo, description);
    }
    else
    {
        fprintf(stderr, "%s %d: Warning %x\n", file, line, errNo);
    }
}

static void printMsg(const char* msg)
{
    puts(msg);
}

#define CAT_SUCCESS 0
#define CAT_MSG 0x80000000

#define CAT_ERROR_CLI -1
#define CAT_ERROR_FILE -2
#define CAT_ERROR_JSON -3
#define CAT_ERROR_EC_SYSTEM -4
#define CAT_ERROR_WINDOW -5
#define CAT_ERROR_SHADER -6

#define CAT_WARNING_CLI 1
#define CAT_WARNING_FILE 2
#define CAT_WARNING_JSON 3
#define CAT_WARNING_EC_SYSTEM 4
#define CAT_WARNING_WINDOW 5
#define CAT_WARNING_SHADER 6

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //ERROR_H
