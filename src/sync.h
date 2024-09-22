#ifndef SYNC_H
#define SYNC_H
#include <stdlib.h>

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
typedef CRITICAL_SECTION LockImpl;

typedef LockImpl *Lock;

inline Lock NewLock() 
{
    Lock result = (Lock)malloc(sizeof(LockImpl));
    InitializeCriticalSection(result);
    return result;
};
inline void DestroyLock(Lock lock)
{
    DeleteCriticalSection(lock);
    free(lock);
}
inline void EnterLock(Lock lock)
{
    EnterCriticalSection(lock);
}
inline void ExitLock(Lock lock)
{
    LeaveCriticalSection(lock);
}

#endif

#ifdef POSIX
#include "pthreads.h"
typedef pthread_mutex_t LockImpl;

inline Lock NewLock() 
{
    Lock result = (Lock)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(result, NULL);
    return result;
}
inline void DestroyLock(Lock lock)
{
    pthread_mutex_destroy(lock);
}
inline void EnterLock(Lock lock)
{
    pthread_mutex_lock(lock);
}
inline void ExitLock(Lock lock)
{
    pthread_mutex_unlock(lock);
}

#endif

#endif