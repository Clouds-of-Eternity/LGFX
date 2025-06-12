#ifndef SYNC_H
#define SYNC_H
#include <stdlib.h>

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
typedef CRITICAL_SECTION LockImpl;

typedef struct 
{
    LockImpl *lockPointer;
    bool isLocked;
} Lock;

static inline Lock NewLock() 
{
    Lock result = {(LockImpl *)malloc(sizeof(LockImpl)), false};
    InitializeCriticalSection(result.lockPointer);
    return result;
};
static inline void DestroyLock(Lock *lock)
{
    DeleteCriticalSection(lock->lockPointer);
    free(lock->lockPointer);
}
static inline void EnterLock(Lock *lock)
{
    if (!lock->isLocked)
    {
        EnterCriticalSection(lock->lockPointer);
        lock->isLocked = true;
    }
}
static inline void ExitLock(Lock *lock)
{
    if (lock->isLocked)
    {
        LeaveCriticalSection(lock->lockPointer);
        lock->isLocked = false;
    }
}

#endif

#ifdef POSIX
#include <pthread.h>
typedef pthread_mutex_t LockImpl;

typedef struct 
{
    LockImpl *lockPointer;
    bool isLocked;
} Lock;

static inline Lock NewLock() 
{
    Lock result = {(LockImpl *)malloc(sizeof(pthread_mutex_t)), false};
    pthread_mutex_init(result.lockPointer, NULL);
    return result;
}
static inline void DestroyLock(Lock *lock)
{
    pthread_mutex_destroy(lock->lockPointer);
    free(lock->lockPointer);
}
static inline void EnterLock(Lock *lock)
{
    if (!lock->isLocked)
    {
        pthread_mutex_lock(lock->lockPointer);
        lock->isLocked = true;
    }
}
static inline void ExitLock(Lock *lock)
{
    if (lock->isLocked)
    {
        pthread_mutex_unlock(lock->lockPointer);
        lock->isLocked = false;
    }
}

#endif

#endif