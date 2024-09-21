#ifndef SYNC_H
#define SYNC_H

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
typedef CRITICAL_SECTION Mutex;

inline Mutex NewMutex() 
{
    Mutex result;
    InitializeCriticalSection(&result);
    return result;
};
inline void DestroyMutex(Mutex *mutex)
{
    DeleteCriticalSection(mutex);
}
inline void EnterMutex(Mutex *mutex)
{
    EnterCriticalSection(mutex);
}
inline void ExitMutex(Mutex *mutex)
{
    LeaveCriticalSection(mutex);
}

#endif

#ifdef POSIX
#include "pthreads.h"
typedef pthread_mutex_t Mutex;

inline Mutex NewMutex() 
{
    Mutex result;
    pthread_mutex_init(&result, NULL);
    return result;
}
inline void DestroyMutex(Mutex *mutex)
{
    pthread_mutex_destroy(mutex);
}
inline void EnterMutex(Mutex *mutex)
{
    pthread_mutex_lock(mutex);
}
inline void ExitMutex(Mutex *mutex)
{
    pthread_mutex_unlock(mutex);
}

#endif

#endif