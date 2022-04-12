#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else //_WIN32
#include <unistd.h>
#endif //_WIN32

#include "util/globalDefs.h"
#include "rpmalloc/rpmalloc.h"
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
    
    sceneManagerCreate(sceneMan, 1);
    sceneManagerRegisterColumnSys(sceneMan, &ENTITY_SYSTEM, COMPONENT(Entity), true);
    
//     while(running)
//     {        
//         sceneManagerFrame(sceneMan, 0.016f);
//     }
    
    sceneManagerDestroy(sceneMan);
    
    linCleanup();
    rpfree(linBuffer);
    
    CHECK_LEAKS();
    rpmalloc_finalize();
    
    return 0;
}
