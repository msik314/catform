#ifndef UTIL_MACROS_H
#define UTIL_MACROS_H

#include <stdint.h>

#define AS(TYPE, VALUE) (*(TYPE*)&VALUE)

#define SIGN(X) (((X) > 0) - ((X) < 0))
#define ABS(X) (X) * SIGN(X)

#define MIN(X, Y) ((X) <= (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) >= (Y) ? (X) : (Y))

#define CLAMP(X, A, B) MIN(MAX(X, A), B)

#define ROUND_UP_TO_POW2(NUM, POW2) (((NUM) + (POW2) - 1) & (~(POW2) + 1))

#define OFFSETOF(TYPE, MEMBER) ((uint64_t)&(((TYPE*)NULL)->MEMBER))

#define BYTE_ORDER_MARK 0x5f333231
#define FILE_TERMINATOR 0x444e4521

#if defined(__GNUC__)

#define PACKED(...) __VA_ARGS__ __attribute__((packed))

#define ALIGNED(X, ...) __VA_ARGS__ __attribute__((aligned (X)))

#define POINTER_MASK 0x0000ffffffffffff

#define EXPECT(EXPR, VAL) __builtin_expect(EXPR, VAL)

static inline int32_t firstSet(uint32_t word)
{
    return __builtin_ffs(word) - 1;
}

static inline int32_t lastSet(uint32_t word)
{
    int32_t bit = word ? 32 - __builtin_clz(word) : 0;
    return bit - 1;
}

static inline int32_t firstSet64(uint64_t word)
{
    return __builtin_ffsll(word) - 1;
}

static inline int32_t lastSet64(uint64_t word)
{
    int32_t bit = word ? 64 - __builtin_clzll(word) : 0;
    return bit - 1;
}


#elif defined(_MSC_VER)//__GNUC__/_MSC_VER

#define PACKED(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))

#define ALIGNED(X, ...) alignas(X) __VA_ARGS__

#define EXPECT(EXPR, VAL) (EXPR)

#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse64)
#pragma intrinsic(_BitScanForwar64)

static inline int32_t firstSet(uint32_t word)
{
    uint32_t index;
    return _BitScanForward(&index, word) ? index : -1;
}

static inline int32_t lastSet(uint32_t word)
{
    uint32_t index;
    return _BitScanReverse(&index, word) ? index : -1;
}

static inline int32_t firstSet64(uint64_t word)
{
    uint32_t index;
    return _BitScanForward64(&index, word) ? index : -1;
}

static inline int32_t lastSet64(uint64_t word)
{
    uint32_t index;
    return _BitScanReverse64(&index, word) ? index : -1;
}

#else //__GNUC__/_MSC_VER

#error "Unsupported Compiler.  Try using gcc/g++, clang/clang++/clang-cl, or MSVC."

#endif //__GNUC__/_MSC_VER

#endif //UTIL_MACROS_H
