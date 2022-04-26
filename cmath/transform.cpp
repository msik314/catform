#include "util/globalDefs.h"
#include "cmath/transform.h"

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "cmath/cVec.h"

#ifdef __restrict__
#define RESTRICT __restrict__
#else
#define RESTRICT
#endif //__restrict__

void transformCompose(const Transform* transform, Mat4* RESTRICT outMatrix)
{
    register float twoXX = 2 * transform->rotation.x * transform->rotation.x;
    register float twoXY = 2 * transform->rotation.x * transform->rotation.y;
    register float twoXZ = 2 * transform->rotation.x * transform->rotation.z;
    register float twoXW = 2 * transform->rotation.x * transform->rotation.w;
    register float twoYY = 2 * transform->rotation.y * transform->rotation.y;
    register float twoYZ = 2 * transform->rotation.y * transform->rotation.z;
    register float twoYW = 2 * transform->rotation.y * transform->rotation.w;
    register float twoZZ = 2 * transform->rotation.z * transform->rotation.z;
    register float twoZW = 2 * transform->rotation.z * transform->rotation.w;
    
    outMatrix->data[0] = transform->scale.x * (1 - twoYY - twoZZ);
    outMatrix->data[1] = transform->scale.x * (twoXY + twoZW);
    outMatrix->data[2] = transform->scale.x * (twoXZ - twoYW);
    outMatrix->data[3] = 0;
    outMatrix->data[4] = transform->scale.y * (twoXY - twoZW);
    outMatrix->data[5] = transform->scale.y * (1 - twoXX - twoZZ);
    outMatrix->data[6] = transform->scale.y * (twoYZ + twoXW);
    outMatrix->data[7] = 0;
    outMatrix->data[8] = transform->scale.z * (twoXZ + twoYW);
    outMatrix->data[9] = transform->scale.z * (twoYZ - twoXW);
    outMatrix->data[10] = transform->scale.z * (1 - twoXX - twoYY);
    outMatrix->data[11] = 0;
    outMatrix->data[12] = transform->position.x;
    outMatrix->data[13] = transform->position.y;
    outMatrix->data[14] = transform->position.z;
    outMatrix->data[15] = 1;
}

void transformCompose(const Transform* transform, glm::mat4* RESTRICT outMatrix)
{
    register float twoXX = 2 * transform->rotation.x * transform->rotation.x;
    register float twoXY = 2 * transform->rotation.x * transform->rotation.y;
    register float twoXZ = 2 * transform->rotation.x * transform->rotation.z;
    register float twoXW = 2 * transform->rotation.x * transform->rotation.w;
    register float twoYY = 2 * transform->rotation.y * transform->rotation.y;
    register float twoYZ = 2 * transform->rotation.y * transform->rotation.z;
    register float twoYW = 2 * transform->rotation.y * transform->rotation.w;
    register float twoZZ = 2 * transform->rotation.z * transform->rotation.z;
    register float twoZW = 2 * transform->rotation.z * transform->rotation.w;
    
    (*outMatrix)[0][0] = transform->scale.x * (1 - twoYY - twoZZ);
    (*outMatrix)[0][1] = transform->scale.x * (twoXY + twoZW);
    (*outMatrix)[0][2] = transform->scale.x * (twoXZ - twoYW);
    (*outMatrix)[0][3] = 0;
    (*outMatrix)[1][0] = transform->scale.y * (twoXY - twoZW);
    (*outMatrix)[1][1] = transform->scale.y * (1 - twoXX - twoZZ);
    (*outMatrix)[1][2] = transform->scale.y * (twoYZ + twoXW);
    (*outMatrix)[1][3] = 0;
    (*outMatrix)[2][0] = transform->scale.z * (twoXZ + twoYW);
    (*outMatrix)[2][1] = transform->scale.z * (twoYZ - twoXW);
    (*outMatrix)[2][2] = transform->scale.z * (1 - twoXX - twoYY);
    (*outMatrix)[2][3] = 0;
    (*outMatrix)[3][0] = transform->position.x;
    (*outMatrix)[3][1] = transform->position.y;
    (*outMatrix)[3][2] = transform->position.z;
    (*outMatrix)[3][3] = 1;
}

bool transformDecompose(const Mat4* matrix, Transform* RESTRICT outTransform)
{
    glm::vec3 skew;
    glm::vec4 perspective;
    return glm::decompose(*(glm::mat4*)matrix, *((glm::vec3*)&outTransform->scale), *(glm::quat*)&outTransform->rotation, *(glm::vec3*)&outTransform->position, skew, perspective);
}

bool transformDecompose(const glm::mat4* matrix, Transform* RESTRICT outTransform)
{
    glm::vec3 skew;
    glm::vec4 perspective;
    return glm::decompose(*matrix, *((glm::vec3*)&outTransform->scale), *(glm::quat*)&outTransform->rotation, *(glm::vec3*)&outTransform->position, skew, perspective);
}
