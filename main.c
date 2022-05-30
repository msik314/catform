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
#include "ecs/tableCache.h"
#include "json/jsonData.h"
#include "json/jsonReader.h"
#include "json/jsonWriter.h"
#include "systems/systems.h"
#include "systems/entitySystem.h"
#include "systems/cameraSystem.h"
#include "systems/spriteSystem.h"
#include "systems/renderSystem.h"
#include "systems/playerSystem.h"
#include "systems/aabbSystem.h"
#include "systems/bulletSystem.h"
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

#define NUM_WORKER_THREADS 2

static void* workerThread(volatile void* args)
{
    SceneManager* sceneMan = sceneManagerGetInstance();
    rpmalloc_thread_initialize();
    
    while(atomicLoad32(args))
    {
        sceneManagerFollowFrame(sceneMan);
    }
    
    rpmalloc_thread_finalize();
    return NULL;
}

int32_t main(int argc, char** argv)
{
    ptrdiff_t pathLen;
    SceneManager* sceneMan = sceneManagerGetInstance();
    void* linBuffer;
    
    Window window;
    Input* input = inputGetInstance();
    ResourceMap* resourceMap = resourceMapGetInstance();
    TableCache* tableCache = tableCacheGetInstance();
    JsonData data;
    
    double currentTime;
    double lastTime;
    volatile uint32_t running = 1;
    pthread_t threads[NUM_WORKER_THREADS] = {};
    
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
    
    sceneManagerCreate(sceneMan, 1 + NUM_WORKER_THREADS);
    sceneManagerRegisterColumnSys(sceneMan, &ENTITY_SYSTEM, COMPONENT(Entity), true);
    sceneManagerRegisterSystem(sceneMan, &RENDER_SYSTEM);
    sceneManagerRegisterColumnSys(sceneMan, &PLAYER_SYSTEM, COMPONENT(PlayerComponent), false);
    sceneManagerRegisterColumnSys(sceneMan, &CAMERA_SYSTEM, COMPONENT(CameraComponent), false);
    sceneManagerRegisterColumnSys(sceneMan, &SPRITE_SYSTEM, COMPONENT(SpriteComponent), false);
    sceneManagerRegisterColumnSys(sceneMan, &AABB_SYSTEM, COMPONENT(AabbComponent), false);
    sceneManagerRegisterColumnSys(sceneMan, &BULLET_SYSTEM, COMPONENT(BulletComponent), true);
    glfwInit();
    
    window.width = 1280;
    window.height = 720;
    window.monitor = -1;
    window.vsync = true;
    
    inputCreate(input);
    
    jsonLoadf(&data, "res/config.json");
    configLoadInput(&data);
    jsonDataDestroy(&data);
    
    windowCreate(&window, "Catform");
    gl3wInit();
    
    sceneManagerInit(sceneMan);
    resourceMapCreate(resourceMap);
    tableCacheCreate(tableCache);
    jsonLoadf(&data, "res/test.cat");
    sceneManagerLoadScene(sceneMan, &data);
    jsonDataDestroy(&data);
    
    for(uint32_t i = 0; i < NUM_WORKER_THREADS; ++i)
    {
        pthread_create(&threads[i], NULL, workerThread, &running);
    }
    
    currentTime = 0;
    lastTime = -1.0/60;
    glfwSetTime(0);
    
    while(!windowShouldClose(&window))
    {
        currentTime = glfwGetTime();
        inputPoll(input, window.window);
        sceneManagerFrame(sceneMan, (float)(currentTime - lastTime));

#ifndef NDEBUG
        printf("%f\n", currentTime - lastTime);
#endif //NDEBUG
        
        windowSwapBuffers(&window);
        lastTime = currentTime;
    }
    atomicStore32(&running, 0);
    
    for(uint32_t i = 0; i < NUM_WORKER_THREADS; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    
    tableCacheDestroy(tableCache);
    resourceMapDestroy(resourceMap);
    sceneManagerDestroy(sceneMan);
    windowDestroy(&window);
    
    jsonDataCreate(&data);
    configSaveInput(&data);
    jsonWritef(&data, "res/config.json");
    jsonDataDestroy(&data);
    
    inputDestroy(input);
    glfwTerminate();
    
    linCleanup();
    CAT_FREE(linBuffer);
    
    CHECK_LEAKS();
    rpmalloc_finalize();
    
    return 0;
}
