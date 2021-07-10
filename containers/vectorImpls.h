#include "containers/vectorTypes.h"
VECTOR_IMPL(uint8_t)
VECTOR_IMPL(uint16_t)
VECTOR_IMPL(uint32_t)
VECTOR_IMPL(uint64_t)
VECTOR_IMPL(int8_t)
VECTOR_IMPL(int16_t)
VECTOR_IMPL(int32_t)
VECTOR_IMPL(int64_t)
VECTOR_IMPL(char)
VECTOR_IMPL(bool)
VECTOR_IMPL(float)
VECTOR_IMPL(double)

#include "ecs/ecSystem.h"
VECTOR_IMPL(ECSystem)

#include "ecs/scheduler.h"
VECTOR_IMPL(Job)

#include "json/jsonData.h"
VECTOR_IMPL(JsonValue)
VECTOR_IMPL(JsonObject)
