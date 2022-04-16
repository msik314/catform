#include "util/globalDefs.h"
#include "render/mesh.h"

#include <GL/gl3w.h>

#include "cmath/cVec.h"

void meshCreate(Mesh* mesh, const Vertex* vertices, uint32_t numVertices, const uint16_t* indices, uint32_t numIndices)
{
    mesh->drawCount = numIndices;

    glGenVertexArrays(1, &mesh->vao);
    glBindVertexArray(mesh->vao);

    glGenBuffers(2, mesh->vab);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vab[0]);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertices,  GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vab[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint16_t), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void meshDestroy(Mesh* mesh)
{
    glDeleteBuffers(2, mesh->vab);
    glDeleteVertexArrays(1, &mesh->vao);
}
