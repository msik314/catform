#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else //_WIN32
#include <unistd.h>
#endif //_WIN32

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "util/globalDefs.h"
#include "rpmalloc/rpmalloc.h"
#include "core/window.h"
#include "ecs/sceneManager.h"
#include "systems/systems.h"
#include "systems/entitySystem.h"
#include "util/linalloc.h"

#define LINALLOC_SIZE 1048576

int32_t main(int argc, char** argv)
{
    ptrdiff_t pathLen;
    SceneManager* sceneMan = sceneManagerGetInstance();
    void* linBuffer;
    
    Window window;
    
    pathLen = strrchr(argv[0], '/') - argv[0];
    if(pathLen < 0) pathLen = strrchr(argv[0], '\\') - argv[0];
    
    if(pathLen > 0)
    {
        char pathDir[pathLen + 1];
        memcpy(pathDir, argv[0], pathLen);
        pathDir[pathLen] = 0;
#ifdef _WIN32
        SetCurrentDirectory(pathDir);
#else //_WIN32
        chdir(pathDir);
#endif //_WIN32
    }
    
    rpmalloc_initialize();
    linBuffer = rpmalloc(LINALLOC_SIZE);
    linInit(linBuffer, LINALLOC_SIZE);
    
    glfwInit();
    gl3wInit();
    
    window.width = 1280;
    window.height = 720;
    window.monitor = -1;
    
    windowCreate(&window, "Catform");
    
    sceneManagerCreate(sceneMan, 1);
    sceneManagerRegisterColumnSys(sceneMan, &ENTITY_SYSTEM, COMPONENT(Entity), true);
    
    while(!windowShouldClose(&window))
    {
        glfwPollEvents();
        sceneManagerFrame(sceneMan, 0.016f);
        windowSwapBuffers(&window);
    }
    
    sceneManagerDestroy(sceneMan);
    
    windowDestroy(&window);
    
    glfwTerminate();
    
    linCleanup();
    rpfree(linBuffer);
    
    CHECK_LEAKS();
    rpmalloc_finalize();
    
    return 0;
}
