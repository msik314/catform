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
#include "cmath/cVec.h"
#include "render/mesh.h"
#include "render/shader.h"
#include "render/shaderSrc.h"
#include "render/texture.h"
#include "render/texMan.h"

#define LINALLOC_SIZE 1048576
#define BLUE 0x00, 0x00, 0xff, 0xff
#define GREEN 0x00, 0xff, 0x00, 0xff

const Vertex vertices[] = 
{
    {-0.5f, 0.5f, 0.0f, 0.0f},
    {-0.5f, -0.5f, 0.0f, 1.0f},
    {0.5f, -0.5f, 1.0f, 1.0f},
    {0.5f, 0.5f, 1.0f, 0.0f}
};

const uint16_t indices[] = {0, 1, 2, 2, 3, 0};

const uint8_t texData[] = 
{
    BLUE, GREEN, BLUE, GREEN,
    GREEN, BLUE, GREEN, BLUE,
    BLUE, GREEN, BLUE, GREEN,
    GREEN, BLUE, GREEN, BLUE
};

int32_t main(int argc, char** argv)
{
    ptrdiff_t pathLen;
    SceneManager* sceneMan = sceneManagerGetInstance();
    void* linBuffer;
    
    Window window;
    Mesh mesh;
    Shader shader;
    Mat4 transform = MAT4_IDENTITY;
    TexMan texMan;
    Tag texName;
    Texture texture;
    
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
    
    glfwInit();
    
    window.width = 1280;
    window.height = 720;
    window.monitor = -1;
    
    windowCreate(&window, "Catform");
    gl3wInit();
    
    meshCreate(&mesh, vertices, sizeof(vertices)/sizeof(Vertex), indices, sizeof(indices)/sizeof(uint16_t));
    shaderCreate(&shader, VERT_SRC, FRAG_SRC);
    
    texManCreate(&texMan);
    
    tagSet(&texName, "checkBoard");
    texture = texManLoad(&texMan, texName, texData, 4, 4);
    
    while(!windowShouldClose(&window))
    {
        glfwPollEvents();
        sceneManagerFrame(sceneMan, 0.016f);
        shaderBind(&shader);
        shaderUniformMat4(0, &transform);
        shaderUniform4f(1, (Vec4){1, 1, 1, 1});
        shaderUniform1i(2, texture);
        
        meshDraw(&mesh, 1);
        
        windowSwapBuffers(&window);
    }
    
    texManDestroy(&texMan);
    
    sceneManagerDestroy(sceneMan);
    
    windowDestroy(&window);
    
    glfwTerminate();
    
    linCleanup();
    rpfree(linBuffer);
    
    CHECK_LEAKS();
    rpmalloc_finalize();
    
    return 0;
}
