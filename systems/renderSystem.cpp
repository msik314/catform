#include "util/globalDefs.h"
#include "systems/renderSystem.h"

#include <gl/gl3w.h>
#include <glm/glm.hpp>

#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/object.h"
#include "ecs/pointerMap.h"
#include "ecs/sceneManager.h"
#include "components/component.h"
#include "components/entity.h"
#include "components/spriteComponent.h"
#include "components/cameraComponent.h"
#include "render/mesh.h"
#include "render/shader.h"
#include "render/shaderSrc.h"
#include "render/texture.h"
#include "render/texMan.h"
#include "util/atomics.h"

#ifndef CAT_MALLOC
#include <stdlib.h>

#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

const Vertex vertices[] = 
{
    {-0.5f, 0.5f, 0.0f, 0.0f},
    {-0.5f, -0.5f, 0.0f, 1.0f},
    {0.5f, -0.5f, 1.0f, 1.0f},
    {0.5f, 0.5f, 1.0f, 0.0f}
};

const uint16_t indices[] = {0, 1, 2, 2, 3, 0};

const JobDependency RENDER_READY_DEPS = {};
const JobDependency RENDER_COPY_DEPS = {};
const JobDependency RENDER_DESTROY_DEPS = {};

const ECSystem RENDER_SYSTEM =
{
    renderSysInit,
    renderSysDestroy,
    
    NULL,
    
    NULL, &RENDER_READY_DEPS,
    renderSysUpdate,
    NULL, &RENDER_COPY_DEPS,
    NULL, &RENDER_DESTROY_DEPS,
    
    NULL,
    NULL
};

typedef struct
{
    Mesh mesh;
    Shader shader;
}
Renderer;

void renderSysInit(ECSystem* self)
{
    Renderer* renderer = (Renderer*)CAT_MALLOC(sizeof(Renderer));
    meshCreate(&renderer->mesh, vertices, sizeof(vertices)/sizeof(Vertex), indices, sizeof(indices)/sizeof(uint16_t));
    shaderCreate(&renderer->shader, VERT_SRC, FRAG_SRC);
    shaderBind(&renderer->shader);
    
    glEnable(GL_DEPTH_TEST);
    self->state = renderer;
    texManCreate(texManGetInstance());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void renderSysDestroy(ECSystem* self)
{
    Renderer* renderer = (Renderer*)self->state;
    
    texManDestroy(texManGetInstance());
    shaderDestroy(&renderer->shader);
    meshDestroy(&renderer->mesh);
    CAT_FREE(renderer);
}

void renderSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime)
{
    const SpriteComponent* spriteComponents = getComponentsConst(columns, SpriteComponent);
    uint32_t numSprites = getNumComponents(columns, SpriteComponent);
    const Entity* entities = getComponentsConst(columns, Entity);
    const PointerMap* map = sceneManagerGetMap(sceneManagerGetInstance());
    const CameraComponent* cameras = getComponentsConst(columns, CameraComponent);
    uint32_t numCameras = getNumComponents(columns, CameraComponent);
    glm::mat4 transform;
    glm::mat4 vp(1);
    uint32_t parent;
    register Renderer* renderer = (Renderer*)self->state;
    
    for(uint32_t i = 0; i < numCameras; ++i)
    {
        if(!(atomicLoad32(&cameras[i].self.flags) & OBJECT_FLAG_DISABLED))
        {
            parent = pointerMapGet(map, cameras[i].self.parent);
            transformCompose(&entities[parent].transform, &transform);
            vp = AS(glm::mat4, cameras[i].projection) * glm::inverse(transform);
            break;
        }
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for(uint32_t i = 0; i < numSprites; ++i)
    {
        
        parent = pointerMapGet(map, spriteComponents[i].self.parent);
        if(parent == INVALID_OBJECT) continue;
        
        transformCompose(&entities[parent].transform, &transform);
        transform = vp * transform;
        shaderUniformMat4(0, (Mat4*)&transform);
        shaderUniform4f(1, spriteComponents[i].tint);
        shaderUniform1i(2, spriteComponents[i].texture);
    
        meshDraw(&renderer->mesh, 1);
    }
}
