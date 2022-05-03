#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus
    
typedef struct
{
    void* window;
    void* context;
    uint32_t width;
    uint32_t height;
    int32_t monitor;
    bool vsync;
}
Window;

void windowCreate(Window* window, const char* title);
void windowDestroy(Window* window);

int32_t windowShouldClose(Window* window);
int32_t windowIsMinimized(Window* window);
void windowSwapBuffers(Window* window);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //WINDOW_H
