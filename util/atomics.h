#ifndef ATOMICS_H
#define ATOMICS_H

#if defined (__alpha__) || defined (__ia64__) || defined (__x86_64__) \
    || defined (_WIN64) || defined (__LP64__) || defined (__LLP64__)
#define _64BIT
#endif

#include <stdint.h>    

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#if defined (__GNUC__)

#define atomicLoad8(ptr) __atomic_load_n((uint8_t*)(ptr), __ATOMIC_RELAXED)
#define atomicStore8(ptr, op) __atomic_exchange_n((uint8_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchAdd8(ptr, op) __atomic_fetch_add((uint8_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchSub8(ptr, op) __atomic_fetch_sub((uint8_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchAnd8(ptr, op) __atomic_fetch_and((uint8_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchOr8(ptr, op) __atomic_fetch_or((uint8_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchXor8(ptr, op) __atomic_fetch_xor((uint8_t*)(ptr), op, __ATOMIC_RELAXED)
#define compareAndSwap8(ptr, expected, desired) __atomic_compare_exchange_n((uint8_t*)(ptr), (uint8_t*)(expected), (uint8_t)(desired), 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED)

#define atomicLoad16(ptr) __atomic_load_n((uint16_t*)(ptr), __ATOMIC_RELAXED)
#define atomicStore16(ptr, op) __atomic_exchange_n((uint16_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchAdd16(ptr, op) __atomic_fetch_add((uint16_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchSub16(ptr, op) __atomic_fetch_sub((uint16_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchAnd16(ptr, op) __atomic_fetch_and((uint16_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchOr16(ptr, op) __atomic_fetch_or((uint16_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchXor16(ptr, op) __atomic_fetch_xor((uint16_t*)(ptr), op, __ATOMIC_RELAXED)
#define compareAndSwap16(ptr, expected, desired) __atomic_compare_exchange_n((uint16_t*)(ptr), (uint16_t*)(expected), (uint16_t)(desired), 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED)

#define atomicLoad32(ptr) __atomic_load_n((uint32_t*)(ptr), __ATOMIC_RELAXED)
#define atomicStore32(ptr, op) __atomic_exchange_n((uint32_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchAdd32(ptr, op) __atomic_fetch_add((uint32_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchSub32(ptr, op) __atomic_fetch_sub((uint32_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchAnd32(ptr, op) __atomic_fetch_and((uint32_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchOr32(ptr, op) __atomic_fetch_or((uint32_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchXor32(ptr, op) __atomic_fetch_xor((uint32_t*)(ptr), op, __ATOMIC_RELAXED)
#define compareAndSwap32(ptr, expected, desired) __atomic_compare_exchange_n((uint32_t*)(ptr), (uint32_t*)(expected), (uint32_t)(desired), 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED)

#define atomicLoad64(ptr) __atomic_load_n((uint64_t*)(ptr), __ATOMIC_RELAXED)
#define atomicStore64(ptr, op) __atomic_exchange_n((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchAdd64(ptr, op) __atomic_fetch_add((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchSub64(ptr, op) __atomic_fetch_sub((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchAnd64(ptr, op) __atomic_fetch_and((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchOr64(ptr, op) __atomic_fetch_or((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchXor64(ptr, op) __atomic_fetch_xor((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define compareAndSwap64(ptr, expected, desired) __atomic_compare_exchange_n((uint64_t*)(ptr), (uint64_t*)(expected), (uint64_t)(desired), 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED)

#define atomicLoadPtr(ptr) __atomic_load_n((uint64_t*)(ptr), __ATOMIC_RELAXED)
#define atomicStorePtr(ptr, op) __atomic_exchange_n((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchAddPtr(ptr, op) __atomic_fetch_add((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchSubPtr(ptr, op) __atomic_fetch_sub((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchAndPtr(ptr, op) __atomic_fetch_and((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchOrPtr(ptr, op) __atomic_fetch_or((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define fetchXorPtr(ptr, op) __atomic_fetch_xor((uint64_t*)(ptr), op, __ATOMIC_RELAXED)
#define compareAndSwapPtr(ptr, expected, desired) __atomic_compare_exchange_n((uint64_t*)(ptr), (uint64_t*)(expected), (uint64_t)(desired), 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED)

static inline bool compareAndSwap128(volatile void *ptr, void* expected, uint64_t desiredLow, uint64_t desiredHigh)
{
    bool result = false;
    uint64_t* e = expected;
    
    __asm__ volatile("lock cmpxchg16b %1"
        : "=@ccz"(result), "+m"(*(uint64_t*)ptr), "+a"(e[0]), "+d"(e[1])
        : "b"(desiredLow), "c"(desiredHigh)
        : "cc");
    
    return result;
}

#elif defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(_InterlockedAnd8, _InterlockedOr8, _InterlockedXor8, _InterlockedExchangeAdd8, _InterlockedExchange8, _InterlockedCompareExchange8)
#pragma intrinsic(_InterlockedAnd16, _InterlockedOr16, _InterlockedXor16, _InterlockedExchangeAdd16, _InterlockedExchange16, _InterlockedCompareExchange16)
#pragma intrinsic(_InterlockedAnd, _InterlockedOr, _InterlockedXor, _InterlockedExchangeAdd, _InterlockedExchange, _InterlockedCompareExchange)
#pragma intrinsic(_InterlockedAnd64, _InterlockedOr64, _InterlockedXor64, _InterlockedExchangeAdd64, _InterlockedExchange64, _InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedCompareExchange128)

static inline bool _cas8(volatile uint8_t* ptr, uint8_t* expected, uint8_t desired)
{
    uint8_t ex  = *expected;
    *expected = (uint8_t)_InterlockedCompareExchange8((volatile char*)ptr, desired, *expected);
    return ex == *expected;
}

static inline bool _cas16(volatile uint16_t* ptr, uint16_t* expected, uint16_t desired)
{
    uint16_t ex  = *expected;
    *expected = (uint16_t)_InterlockedCompareExchange16((volatile short*)ptr, desired, *expected);
    return ex == *expected;
}

static inline bool _cas32(volatile uint32_t* ptr, uint32_t* expected, uint32_t desired)
{
    uint32_t ex  = *expected;
    *expected = (uint32_t)_InterlockedCompareExchange((volatile long*)ptr, desired, *expected);
    return ex == *expected;
}

static inline bool _cas64(volatile uint64_t* ptr, uint64_t* expected, uint64_t desired)
{
    uint64_t ex  = *expected;
    *expected = (uint64_t)_InterlockedCompareExchange64((volatile long long*)ptr, desired, *expected);
    return ex == *expected;
}

#define atomicLoad8(ptr) (uint8_t)_InterlockedOr8((volatile char*)(ptr), '\x0')
#define atomicStore8(ptr, op) (uint8_t)_InterlockedExchange8((volatile char*)(ptr), op)
#define fetchAdd8(ptr, op) (uint8_t)_InterlockedExchangeAdd8((volatile char*)(ptr), op)
#define fetchSub8(ptr, op) (uint8_t)_InterlockedExchangeAdd8((volatile char*)(ptr), -(op))
#define fetchAnd8(ptr, op) (uint8_t)_InterlockedAnd8((volatile char*)(ptr), op)
#define fetchOr8(ptr, op) (uint8_t)_InterlockedOr8((volatile char*)(ptr), op)
#define fetchXor8(ptr, op) (uint8_t)_InterlockedXor8((volatile char*)(ptr), op)
#define compareAndSwap8(ptr, expected, desired) _cas8((volatile uint8_t*)(ptr), (uint8_t*)(expected), (uint8_t)(desired))

#define atomicLoad16(ptr) (uint16_t)_InterlockedOr16((volatile short*)(ptr), '\x0')
#define atomicStore16(ptr, op) (uint16_t)_InterlockedExchange16((volatile short*)(ptr), op)
#define fetchAdd16(ptr, op) (uint16_t)_InterlockedExchangeAdd16((volatile short*)(ptr), op)
#define fetchSub16(ptr, op) (uint16_t)_InterlockedExchangeAdd16((volatile short*)(ptr), -(op))
#define fetchAnd16(ptr, op) (uint16_t)_InterlockedAnd16((volatile short*)(ptr), op)
#define fetchOr16(ptr, op) (uint16_t)_InterlockedOr16((volatile short*)(ptr), op)
#define fetchXor16(ptr, op) (uint16_t)_InterlockedXor16((volatile short*)(ptr), op)
#define compareAndSwap16(ptr, expected, desired) _cas16((volatile uint16_t*)(ptr), (uint16_t*)(expected), (uint16_t)(desired))

#define atomicLoad32(ptr) (uint32_t)_InterlockedOr((volatile long*)(ptr), 0)
#define atomicStore32(ptr, op) (uint32_t)_InterlockedExchange((volatile long*)(ptr), op)
#define fetchAdd32(ptr, op) (uint32_t)_InterlockedExchangeAdd((volatile long*)(ptr), op)
#define fetchSub32(ptr, op) (uint32_t)_InterlockedExchangeAdd((volatile long*)(ptr), -(op))
#define fetchAnd32(ptr, op) (uint32_t)_InterlockedAnd((volatile long*)(ptr), op)
#define fetchOr32(ptr, op) (uint32_t)_InterlockedOr((volatile long*)(ptr), op)
#define fetchXor32(ptr, op) (uint32_t)_InterlockedXor((volatile long*)(ptr), op)
#define compareAndSwap32(ptr, expected, desired) _cas32((volatile uint32_t*)(ptr), (uint32_t*)(expected), (uint32_t)(desired))

#define atomicLoad64(ptr) (uint64_t)_InterlockedOr64((volatile long long*)(ptr), 0)
#define atomicStore64(ptr, op) (uint64_t)_InterlockedExchange64((volatile long long*)(ptr), op)
#define fetchAdd64(ptr, op) (uint64_t)_InterlockedExchangeAdd64((volatile long long*)(ptr), op)
#define fetchSub64(ptr, op) (uint64_t)_InterlockedExchangeAdd64((volatile long long*)(ptr), -(op))
#define fetchAnd64(ptr, op) (uint64_t)_InterlockedAnd64((volatile long long*)(ptr), op)
#define fetchOr64(ptr, op) (uint64_t)_InterlockedOr64((volatile long long*)(ptr), op)
#define fetchXor64(ptr, op) (uint64_t)_InterlockedXor64((volatile long long*)(ptr), op)
#define compareAndSwap64(ptr, expected, desired) _cas64((volatile uint64_t*)(ptr), (uint64_t*)(expected), (uint64_t)(desired))

#define atomicLoadPtr(ptr) (uint64_t)_InterlockedOr64((volatile long long*)(ptr), 0)
#define atomicStorePtr(ptr, op) (uint64_t)_InterlockedExchange64((volatile long long*)(ptr), op)
#define fetchAddPtr(ptr, op) (uint64_t)_InterlockedExchangeAdd64((volatile long long*)(ptr), op)
#define fetchSubPtr(ptr, op) (uint64_t)_InterlockedExchangeAdd64((volatile long long*)(ptr), -(op))
#define fetchAndPtr(ptr, op) (uint64_t)_InterlockedAnd64((volatile long long*)(ptr), op)
#define fetchOrPtr(ptr, op) (uint64_t)_InterlockedOr64((volatile long long*)(ptr), op)
#define fetchXorPtr(ptr, op) (uint64_t)_InterlockedXor64((volatile long long*)(ptr), op)
#define compareAndSwapPtr(ptr, expected, desired) _cas64((volatile uint64_t*)(ptr), (uint64_t*)(expected), (uint64_t)(desired))

static inline bool compareAndSwap128(volatile void *ptr, void* expected, uint64_t desiredLow, uint64_t desiredHigh)
{
    int64_t* e = expected;
    volatile int64_t* res = ptr;
    
    return (bool)_InterlockedCompareExchange128(res, desiredHigh, desiredLow, e);
}

#else //__GNUC__/_MSC_VER

#error "Unsupported Compiler.  Try using gcc/g++, clang/clang++/clang-cl, or MSVC."

#endif //__GNUC__/_MSC_VER

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //ATOMICS_H
