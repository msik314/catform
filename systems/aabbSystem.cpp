#include "util/globalDefs.h"
#include "systems/aabbSystem.h"

#include <stdlib.h>
#include <string.h>
#include <glm/glm.hpp>
#include "core/input.h"
#include "ecs/ecSystem.h"
#include "ecs/ectColumn.h"
#include "ecs/sceneManager.h"
#include "ecs/pointerMap.h"
#include "ecs/object.h"
#include "ecs/phase.h"
#include "systems/systems.h"
#include "systems/entitySystem.h"
#include "util/atomics.h"
#include "util/linalloc.h"
#include "util/utilMacros.h"
#include "cmath/transform.h"

#ifndef CAT_MALLOC
#define CAT_MALLOC(SIZE) malloc(SIZE)
#define CAT_FREE(PTR) free(PTR)
#endif //CAT_MALLOC

const JobDependency AABB_READY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(AabbComponent), PHASE_PARENT)}};
const JobDependency AABB_COPY_DEPS = {1, {MAKE_JOB_ID(SYSTEM(AabbComponent), PHASE_UPDATE)}};

const JobDependency AABB_DESTROY_DEPS = {1, {MAKE_JOB_ID(COMPONENT(AabbComponent), PHASE_MARK)}};

static void aabbColCreate(ECTColumn* column){ectColumnCreate(AabbComponent)((ECTColumn(AabbComponent)*)column);}

const ECSystem AABB_SYSTEM =
{
    aabbSysInit,
    aabbSysDestroy,
    
    aabbColCreate,
    
    aabbCompReady, &AABB_READY_DEPS,
    aabbSysUpdate,
    aabbCompCopy, &AABB_COPY_DEPS,
    aabbCompDestroy, &AABB_DESTROY_DEPS,
    
    aabbCompReadyAll,
    aabbCompDestroyAll
};

typedef struct
{
    Collection(CollisionHalf) openPool;
    Collection(Collision) collisions;
    Collection(CollisionEdge) edges;
    Hashmap(ObjectID, EdgePair) pairMap;
}
ColliderCache;

static inline void colliderCacheCreate(ColliderCache* cache)
{
    collectionCreate(CollisionHalf)(&cache->openPool);
    collectionCreate(Collision)(&cache->collisions);
    collectionCreate(CollisionEdge)(&cache->edges);
    hashmapCreate(ObjectID, EdgePair)(&cache->pairMap);
}

static inline void colliderCacheDestroy(ColliderCache* cache)
{
    hashmapDestroy(ObjectID, EdgePair)(&cache->pairMap);
    collectionDestroy(CollisionEdge)(&cache->edges);
    collectionDestroy(Collision)(&cache->collisions);
    collectionDestroy(CollisionHalf)(&cache->openPool);
}

static inline void setEdgeIdx(Hashmap(ObjectID, EdgePair)* pairMap, const CollisionEdge* edge, uint32_t index)
{
    EdgePair pair;
    hashmapGet(ObjectID, EdgePair)(pairMap, &edge->aabb, &pair);
    
    if(edge->size < 0)
    {
        pair.end = index;
    }
    else
    {
        pair.start = index;
    }
    
    hashmapSet(ObjectID, EdgePair)(pairMap, &edge->aabb, &pair);
}

static inline void colliderCacheRemove(ColliderCache* cache, ObjectID aabb)
{
    EdgePair pair;
    EdgePair tmp;
    if(!hashmapGet(ObjectID, EdgePair)(&cache->pairMap, &aabb, &pair)) return;
    
    collectionRemove(CollisionEdge)(&cache->edges, pair.end);
    if(pair.end != cache->edges.size)
    {
        setEdgeIdx(&cache->pairMap, &cache->edges.data[pair.end], pair.end);
    }
    
    collectionRemove(CollisionEdge)(&cache->edges, pair.start);
    if(pair.start != cache->edges.size)
    {
        setEdgeIdx(&cache->pairMap, &cache->edges.data[pair.start], pair.start);
    }
    
    hashmapRemove(ObjectID, EdgePair)(&cache->pairMap, &aabb);
}

void aabbSysInit(ECSystem* self)
{
    ColliderCache* cache = (ColliderCache*)CAT_MALLOC(sizeof(ColliderCache));
    colliderCacheCreate(cache);
    self->state = cache;
}

void aabbSysDestroy(ECSystem* self)
{
    colliderCacheDestroy((ColliderCache*)self->state);
    CAT_FREE(self->state);
}

void aabbCompReady(ECSystem* self, ECTColumn* column)
{
    ECTColumn(AabbComponent)* aabbs = (ECTColumn(AabbComponent)*)column;
    uint32_t idx = aabbs->components.size;
    uint32_t flags;
    
    while(idx)
    {
        --idx;
        
        flags = atomicLoad32(&aabbs->components.data[idx].self.flags);
        if(!(flags & OBJECT_FLAG_UNREADY))
        {
            return;
        }
        
        fetchAnd32(&aabbs->components.data[idx].self.flags, ~OBJECT_FLAG_UNREADY);
        aabbReady(&aabbs->components.data[idx]);
    }
}

static inline void sortEdges(Collection(CollisionEdge)* edges, Hashmap(ObjectID, EdgePair)* pairMap)
{
    uint32_t j;
    register CollisionEdge tmp;
    for(uint32_t i = 1; i < edges->size; ++i)
    {
        for(j = i; j > 0 && edges->data[j].position.x < edges->data[j - 1].position.x; --j)
        {
            tmp = edges->data[j];
            edges->data[j] = edges->data[j - 1];
            edges->data[j - 1] = tmp;
            setEdgeIdx(pairMap, &edges->data[j], j);
        }
        if(j != i)
        {
            setEdgeIdx(pairMap, &edges->data[j], j);
        }
    }
}

static inline Vec4 transformAabb(const AabbComponent* aabb, const Entity* entities, const PointerMap* map)
{
    glm::mat4 transform;
    glm::mat4 parentTransform;
    glm::vec4 center = {aabb->offset.x, aabb->offset.y, 0, 1};
    glm::vec4 topRight = center + glm::vec4(aabb->size.x, aabb->size.x / 2, 0, 0); 
    glm::vec4 botRight = center + glm::vec4(aabb->size.x, -aabb->size.y / 2, 0, 0);
    
    register uint32_t idx = pointerMapGet(map, aabb->self.parent);
    transformCompose(&entities[idx].transform, &transform);
    
    while(entities[idx].self.parent != INVALID_OBJECT)
    {
        idx = pointerMapGet(map, entities[idx].self.parent);
        transformCompose(&entities[idx].transform, &parentTransform);
        transform = parentTransform * transform;
    }
    
    center = transform * center;
    topRight = transform * topRight;
    botRight = transform * botRight;
    
    topRight.x = ABS(topRight.x);
    topRight.y = ABS(topRight.y);
    botRight.x = ABS(botRight.x);
    botRight.y = ABS(botRight.y);
    
    return (Vec4){center.x, center.y, MAX(topRight.x, botRight.x), MAX(topRight.y, botRight.y)}; 
}

void aabbSysUpdate(ECSystem* self, SysFlags* flags, const ECTColumn* columns, uint32_t numColumns, float deltaTime)
{
    const AabbComponent* aabbs = getComponentsConst(columns, AabbComponent);
    uint32_t numAabbs = getNumComponents(columns, AabbComponent);
    const Entity* entities = getComponentsConst(columns, Entity);
    const PointerMap* map = sceneManagerGetMap(sceneManagerGetInstance());
    EdgePair pair;
    CollisionEdge edge;
    Collision col = {};
    CollisionHalf open = {};
    ColliderCache* cache = (ColliderCache*)self->state;
    AabbFlags* aabbFlags;
    Vec4 transformedBox;
    register uint32_t idx;
    register ObjectID aabb1;
    register ObjectID aabb2;
    register float size1;
    register float size2;
    register float x1;
    register float x2;
    register float xOverlap;
    register float y1;
    register float y2;
    register float ydiff;
    register float yOverlap;
    cache->openPool.size = 0;
    
    for(uint32_t i = 0; i < numAabbs; ++i)
    {
        transformedBox = transformAabb(&aabbs[i], entities, map);
        if(hashmapGet(ObjectID, EdgePair)(&cache->pairMap, &aabbs[i].self.id, &pair))
        {
            transformedBox.z /= 2;
            cache->edges.data[pair.start].position.x = transformedBox.x - transformedBox.z;
            cache->edges.data[pair.start].position.y = transformedBox.y;
            cache->edges.data[pair.start].size = transformedBox.w;
            cache->edges.data[pair.start].aabb = aabbs[i].self.id;
            
            cache->edges.data[pair.end].position.x = transformedBox.x + transformedBox.z;
            cache->edges.data[pair.end].position.y = transformedBox.y;
            cache->edges.data[pair.end].size = -transformedBox.w;
            cache->edges.data[pair.end].aabb = aabbs[i].self.id;
        }
        else
        {
            transformedBox.z /= 2;
            edge.position.x = transformedBox.x - transformedBox.z;
            edge.position.y = transformedBox.y;
            edge.size = transformedBox.w;
            edge.aabb = aabbs[i].self.id;
            pair.start = cache->edges.size;
            collectionAdd(CollisionEdge)(&cache->edges, &edge);
            
            edge.position.x = transformedBox.x + transformedBox.z;
            edge.size *= -1;
            collectionAdd(CollisionEdge)(&cache->edges, &edge);
            pair.end = pair.start + 1;
            
            hashmapSet(ObjectID, EdgePair)(&cache->pairMap, &aabbs[i].self.id, &pair);
        }
    }
    
    sortEdges(&cache->edges, &cache->pairMap);
    
    for(uint32_t i = 0; i < cache->edges.size; ++i)
    {
        if(cache->edges.data[i].size > 0)
        {
            open = (CollisionHalf){};
            open.aabb = cache->edges.data[i].aabb;
            open.index = i;
            collectionAdd(CollisionHalf)(&cache->openPool, &open);
        }
        else
        {
            size2 = -cache->edges.data[i].size;
            x2 = cache->edges.data[i].position.x;
            y2 = cache->edges.data[i].position.y;
            aabb2 = cache->edges.data[i].aabb;
            
            idx = pointerMapGet(map, aabb2);
            col.entity2 = aabbs[idx].self.parent;
            col.aabb2 = aabb2;
            
            for(int32_t j = cache->openPool.size - 1; j > 0; --j)
            {
                idx = cache->openPool.data[j].index;
                aabb1 = cache->edges.data[idx].aabb;
                if(aabb1 == aabb2)
                {
                    collectionRemove(CollisionHalf)(&cache->openPool, j);
                    continue;
                }
                
                size1 = cache->edges.data[idx].size;
                y1 = cache->edges.data[idx].position.y;
                ydiff = y2 - y1;
                yOverlap = size1 + size2 - ABS(ydiff);
                
                if(yOverlap > 0)
                {
                    idx = pointerMapGet(map, aabb1);
                    col.entity1 = aabbs[idx].self.parent;
                    col.aabb1 = aabb1;
                    
                    x1 = cache->edges.data[idx].position.x;
                    xOverlap = x2 - x1;
                    if(xOverlap < yOverlap)
                    {
                        col.normal = (Vec2){(float)SIGN(xOverlap), 0};
                        col.overlap = ABS(xOverlap);
                    }
                    else
                    {
                        col.normal = (Vec2){0, (float)SIGN(ydiff)};
                        col.overlap = yOverlap;
                    }
                    
                    collectionAdd(Collision)(&cache->collisions, &col);
                }
            }
        }
    }
    
    aabbFlags = (AabbFlags*)linalloc(OFFSETOF(AabbFlags, collisions) + cache->collisions.size * sizeof(Collision));
    memcpy(&aabbFlags->collisions, cache->collisions.data, cache->collisions.size * sizeof(Collision));
    *flags = aabbFlags;
}

void aabbCompCopy(ECSystem* self, ECTColumn* column, const SysFlags* flags, uint32_t numFlags, float deltaTime){}

void aabbCompDestroy(ECSystem* self, ECTColumn* column)
{
    ECTColumn(AabbComponent)* aabbs = (ECTColumn(AabbComponent)*)column;
    uint32_t flags;
    
    for(uint32_t i = 0; i < aabbs->components.size; ++i)
    {
        flags = atomicLoad32(&aabbs->components.data[i].self.flags);
        if(flags & OBJECT_FLAG_REMOVE) aabbOnDestroy(&aabbs->components.data[i], self->state);
    }
}

void aabbCompReadyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(AabbComponent)* aabbs = (ECTColumn(AabbComponent)*)column;
    
    for(uint32_t i = 0; i < aabbs->components.size; ++i)
    {
        fetchAnd32(&aabbs->components.data[i].self.flags, ~OBJECT_FLAG_UNREADY);
        aabbReady(&aabbs->components.data[i]);
    }
}

void aabbCompDestroyAll(ECSystem* self, ECTColumn* column)
{
    ECTColumn(AabbComponent)* aabbs = (ECTColumn(AabbComponent)*)column;
    AabbComponent aabb;
    
    for(uint32_t i = 0; i < aabbs->components.size; ++i)
    {
        aabbOnDestroy(&aabbs->components.data[i], self->state);
    }
    
    while(mwQueuePop(AabbComponent)(&aabbs->addQueue, &aabb))
    {
        aabbOnDestroy(&aabb, self->state);
    }
}

void aabbReady(AabbComponent* aabb){}

void aabbOnDestroy(AabbComponent* aabb, void* systemState)
{
    colliderCacheRemove((ColliderCache*)systemState, aabb->self.id);
} 
