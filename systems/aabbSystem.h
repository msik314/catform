#ifndef AABB_SYSTEM_H
#define AABB_SYSTEM_H

#include <stdint.h>
#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/object.h"
#include "components/aabbComponent.h"
#include "containers/hashmap.h"
#include "containers/collection.h"
#include "cmath/cvec.h"
#include "util/utilMacros.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

typedef struct
{
    Vec2 position;
    float size;
    ObjectID aabb;
}
CollisionEdge;

COLLECTION_DECL(CollisionEdge)

typedef struct
{
    ObjectID aabb;
    uint32_t index;
}
CollisionHalf;

COLLECTION_DECL(CollisionHalf)

typedef struct
{
    uint32_t start;
    uint32_t end;
}
EdgePair;

HASHMAP_DECL(ObjectID, EdgePair)

typedef struct
{
    ObjectID entity1;
    ObjectID aabb1;
    ObjectID entity2;
    ObjectID aabb2;
    Vec2 normal;
    float overlap;
}
Collision;

COLLECTION_DECL(Collision)

typedef struct
{
    uint32_t numCollisions;
    Collision collisions[];
}
AabbFlags;

void aabbSysInit(ECSystem* self);
void aabbSysDestroy(ECSystem* self);

void aabbCompReady(ECSystem* self, ECTColumn* column);
void aabbSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime);
void aabbCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime);
void aabbCompDestroy(ECSystem* self, ECTColumn* column);

void aabbCompReadyAll(ECSystem* self, ECTColumn* column);
void aabbCompDestroyAll(ECSystem* self, ECTColumn* column);

void aabbReady(AabbComponent* aabb);
void aabbOnDestroy(AabbComponent* aabb, void* systemState); 

extern const ECSystem AABB_SYSTEM;

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //AABB_SYSTEM_H
