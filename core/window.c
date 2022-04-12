#include "util/globalDefs.h"
#include "core/window.h"

#include <GLFW/glfw3.h>

void windowCreate(Window* window, const char* title)
{
    GLFWmonitor** monitors;
    int32_t numMonitors;
    GLFWmonitor* monitor;
    const GLFWvidmode* vidMode;
    GLFWwindow* glWin;
    
    glfwWindowHint(GLFW_RESIZABLE, 0);
    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CENTER_CURSOR, 1);
    
    if(window->monitor < 0)
    {
        glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
        glWin = glfwCreateWindow(window->width, window->height, title, NULL, NULL);
    }
    else
    {
        monitors = glfwGetMonitors(&numMonitors);
        
        if(window->monitor < numMonitors)
        {
            monitor = monitors[window->monitor];
        }
        else
        {
            window->monitor = 0;
            monitor = glfwGetPrimaryMonitor();
        }
        
        vidMode = glfwGetVideoMode(monitor);
        
        if(window->width < 1 || window->width > vidMode->width)
        {
            window->width = vidMode->width;
        }
        
        if(window->height < 1 || window->height > vidMode->height)
        {
            window->height = vidMode->height;
        }
        
        glfwWindowHint(GLFW_RED_BITS, vidMode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, vidMode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, vidMode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, vidMode->refreshRate);
        
        glWin = glfwCreateWindow(window->width, window->height, title, monitor, NULL);
    }
    glfwMakeContextCurrent(glWin);
    window->window = glWin;
}

void windowDestroy(Window* window)
{
    glfwDestroyWindow((GLFWwindow*)window->window);
}

int32_t windowShouldClose(Window* window)
{
    return glfwWindowShouldClose((GLFWwindow*)window->window);
}

int32_t windowIsMinimized(Window* window)
{
    int32_t width;
    int32_t height;
    
    glfwGetFramebufferSize((GLFWwindow*)window->window, &width, &height);
    
    return width == 0 && height == 0;
}

void windowSwapBuffers(Window* window)
{
    glfwSwapBuffers((GLFWwindow*)window->window);
}
