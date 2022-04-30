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
#include "core/input.h"
#include "core/config.h"
#include "ecs/sceneManager.h"
#include "json/jsonData.h"
#include "json/jsonReader.h"
#include "systems/systems.h"
#include "systems/entitySystem.h"
#include "systems/cameraSystem.h"
#include "systems/spriteSystem.h"
#include "systems/renderSystem.h"
#include "util/linalloc.h"
#include "util/resourceMap.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

#define LINALLOC_SIZE 1048576
#define BLUE 0x00, 0x00, 0xff, 0xff
#define GREEN 0x00, 0xff, 0x00, 0xff

int32_t main(int argc, char** argv)
{
    ptrdiff_t pathLen;
    SceneManager* sceneMan = sceneManagerGetInstance();
    void* linBuffer;
    
    Window window;
    Input* input = inputGetInstance();
    ResourceMap* resourceMap = resourceMapGetInstance();
    JsonData data;
    
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
    linBuffer = CAT_MALLOC(LINALLOC_SIZE);
    linInit(linBuffer, LINALLOC_SIZE);
    
    sceneManagerCreate(sceneMan, 1);
    sceneManagerRegisterColumnSys(sceneMan, &ENTITY_SYSTEM, COMPONENT(Entity), true);
    sceneManagerRegisterColumnSys(sceneMan, &CAMERA_SYSTEM, COMPONENT(CameraComponent), false);
    sceneManagerRegisterColumnSys(sceneMan, &SPRITE_SYSTEM, COMPONENT(SpriteComponent), false);
    sceneManagerRegisterSystem(sceneMan, &RENDER_SYSTEM);
    glfwInit();
    
    window.width = 1280;
    window.height = 720;
    window.monitor = -1;
    
    inputCreate(input);
    
    windowCreate(&window, "Catform");
    gl3wInit();
    
    sceneManagerInit(sceneMan);
    resourceMapCreate(resourceMap);
    
    jsonLoadf(&data, "res/test.cat");
    sceneManagerLoadScene(sceneMan, &data);
    jsonDataDestroy(&data);
    
    jsonLoadf(&data, "res/config.json");
    configLoadInput(&data);
    jsonDataDestroy(&data);
    
    while(!windowShouldClose(&window))
    {
        inputPoll(input, window.window);
        sceneManagerFrame(sceneMan, 0.016f);
        
        printf
        (
            "%.2f, %.2f, %d %d %d\n",
            (double)inputGetAxis(input, 0, 0),
            (double)inputGetAxis(input, 0, 1),
            inputGetButton(input, 0, 0),
            inputGetButton(input, 0, 1),
            inputGetButton(input, 0, 2)
        );
        
        windowSwapBuffers(&window);
    }
    
    resourceMapDestroy(resourceMap);
    sceneManagerDestroy(sceneMan);
    inputDestroy(input);
    windowDestroy(&window);
    glfwTerminate();
    
    linCleanup();
    CAT_FREE(linBuffer);
    
    CHECK_LEAKS();
    rpmalloc_finalize();
    
    return 0;
}
