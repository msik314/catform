#include "containers/mwQueueTypes.h"
MWQUEUE_IMPL(uint8_t)
MWQUEUE_IMPL(uint16_t)
MWQUEUE_IMPL(uint32_t)
MWQUEUE_IMPL(uint64_t)
MWQUEUE_IMPL(int8_t)
MWQUEUE_IMPL(int16_t)
MWQUEUE_IMPL(int32_t)
MWQUEUE_IMPL(int64_t)
MWQUEUE_IMPL(char)
MWQUEUE_IMPL(bool)
MWQUEUE_IMPL(float)
MWQUEUE_IMPL(double)

#include "components/entity.h"
MWQUEUE_IMPL(Entity);

#include "components/spriteComponent.h"
MWQUEUE_IMPL(SpriteComponent);
