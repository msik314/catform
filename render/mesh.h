#ifndef MESH_H
#define MESH_H

#include <stdint.h>
#include <GL/gl3w.h>
#include "cmath/cVec.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef union
{
    Vec4 data;
    struct
    {
        Vec2 position;
        Vec2 texCoord;
    };
}
Vertex;

typedef struct
{
    GLuint vao;
    GLuint vab[2];
    uint32_t drawCount;
}
Mesh;

void meshCreate(Mesh* mesh, Vertex* vertices, uint32_t numVertices, uint16_t* indices, uint32_t numIndices);
void meshDestroy(Mesh* mesh);

static inline void draw(Mesh* mesh, uint32_t numInstances)
{
    glBindVertexArray(mesh->vao);
    glDrawElementsInstanced(GL_TRIANGLES, mesh->drawCount, GL_UNSIGNED_SHORT, NULL, numInstances);
}

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //MESH_H
