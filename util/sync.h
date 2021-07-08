#ifndef SYNC_H
#define SYNC_H

#include <stdint.h>
#include <pthread.h>
#include "util/atomics.h"
#include "util/utilMacros.h"

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#if defined (__GNUC__)

#define testAndSet(ptr) __atomic_test_and_set(ptr, __ATOMIC_RELAXED)
#define atomicClear(ptr) __atomic_clear(ptr, __ATOMIC_RELAXED)

#define SPIN() __asm__ volatile("pause")

#elif defined(_MSC_VER)

#include <intrin.h>
#pragma intrinsic(_interlockedbittestandset, _mm_pause)
    
#define testAndSet(ptr) _interlockedbittestandset((volatile long*)ptr, 0)
#define atomicClear(ptr) atomicStore32(ptr, 0)

#define SPIN() _mm_pause()

#else //__GNUC__/_MSC_VER

#error "Unsupported Compiler.  Try using gcc/g++, clang/clang++/clang-cl, or MSVC."

#endif //__GNUC__/_MSC_VER

/*Mutex*/
//Define the mutex to be a union so it is the same size across compilers
typedef union
{
    volatile bool boolLock;
    volatile int32_t intLock;
}
Mutex;

static inline void mutexCreate(Mutex* mutex){atomicStore32(&mutex->intLock, 0);}
static inline void mutexDestroy(Mutex* mutex){atomicStore32(&mutex->intLock, 0);}

static inline void mutexLock(Mutex* mutex)
{
    do
    {
        while(mutex->intLock){SPIN();}
    }
#if defined (__GNUC__)
    while(testAndSet(&mutex->boolLock));
#elif defined(_MSC_VER)
    while(testAndSet(&mutex->intLock));
#endif //__GNUC__/_MSC_VER
}

static inline bool mutexTryLock(Mutex* mutex)
{
#if defined (__GNUC__)
    return !testAndSet(&mutex->boolLock);
#elif defined(_MSC_VER)
    return !testAndSet(&mutex->intLock);
#endif //__GNUC__/_MSC_VER
}

static inline void mutexUnlock(Mutex* mutex)
{
#if defined (__GNUC__)
    atomicClear(&mutex->boolLock);
#elif defined(_MSC_VER)
    atomicClear(&mutex->intLock);
#endif //__GNUC__/_MSC_VER
}

/*RecursiveMutex*/
typedef volatile uintptr_t RecursiveMutex;

static inline void recursiveMutexCreate(RecursiveMutex* mutex){atomicStorePtr(mutex, 0);}
static inline void recursiveMutexDestroy(RecursiveMutex* mutex){atomicStorePtr(mutex, 0);}

static inline void recursiveMutexLock(RecursiveMutex* mutex, uintptr_t threadId)
{
    if(*mutex == threadId) return;
    uintptr_t zero;
    do
    {
        zero = 0;
        while(*mutex){SPIN();}
    }
    while(!compareAndSwapPtr(mutex, &zero, threadId));
}

static inline bool recursiveMutexTryLock(RecursiveMutex* mutex, uintptr_t threadId)
{
    uintptr_t zero = 0;
    if(*mutex == threadId) return true;
    return compareAndSwapPtr(mutex, &zero, threadId);
}

static inline void recursiveMutexUnlock(RecursiveMutex* mutex, uintptr_t threadId)
{
    if(*mutex != threadId) return;
    atomicStorePtr(mutex, 0);
}

/*Barrier*/
typedef struct
{
    volatile int32_t currentCount;
    int32_t maxCount;
}
Barrier;

static inline void barrierCreate(Barrier* barrier, int32_t threadCount)
{
    barrier->maxCount = threadCount;
    atomicStore32(&barrier->currentCount, 0);
}

static inline void barrierDestroy(Barrier* barrier)
{
    atomicStore32(&barrier->currentCount, -barrier->maxCount);
}

static inline void barrierWait(Barrier* barrier)
{
    uint32_t arrival;
    while((int32_t)atomicLoad32(&barrier->currentCount) < 0){SPIN();}
    arrival = fetchAdd32(&barrier->currentCount, 1);
    
    if(arrival < barrier->maxCount - 1)
    {
        while((int32_t)atomicLoad32(&barrier->currentCount) > 0){SPIN();}
        fetchAdd32(&barrier->currentCount, 1);
    }
    else
    {
        atomicStore32(&barrier->currentCount, -barrier->maxCount + 1);
    }
}

static inline int32_t barrierGetMaxThreads(Barrier* barrier) {return barrier->maxCount;}

/*Semaphore*/
typedef struct
{
        volatile int32_t currentCount;
        int32_t maxCount;
}
Semaphore;

static inline void semaphoreCreate(Semaphore* semaphore, int32_t resourceCount)
{
    atomicStore32(&semaphore->currentCount, resourceCount);
    atomicStore32(&semaphore->maxCount, resourceCount);
}

static inline void semaphoreDestroy(Semaphore* semaphore)
{
    atomicStore32(&semaphore->currentCount, semaphore->maxCount);
}

static inline void semaphoreWait(Semaphore* semaphore, int32_t resourceCount)
{
    int16_t semaphoreCount = semaphore->currentCount;
    do
    {
        while(semaphoreCount < resourceCount)
        {
            SPIN();
            semaphoreCount = semaphore->currentCount;
        }
    }
    while(compareAndSwap32(&semaphore->currentCount, &semaphoreCount, semaphoreCount - resourceCount));
}

static inline void semaphoreSignal(Semaphore* semaphore, int32_t resourceCount)
{
    fetchAdd32(&semaphore->currentCount, resourceCount);
}

#if defined (__GNUC__)
#define threadFence() asm volatile("mfence")
#define memoryBarrier() __sync_synchronize()
#elif defined (_MSC_VER)//__GNUC__/_MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <intrin.h>
#include <windows.h>
#pragma intrinsic(_mm_mfence)
#define threadFence() _mm_mfence()
#define memoryBarrier() MemoryBarrier()
#else //__GNUC__/_MSC_VER
#error "Unsupported Compiler.  Try using gcc/g++, clang/clang++/clang-cl, or MSVC."
#endif //__GNUC__/_MSC_VER

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SYNC_H
