/*
 * Created: 2019-05-23 17:18 +0800
 *
 * Modified: 2019-05-23 17:18 +0800
 *
 * Description: 
 *
 * Author: jh
 */
#ifndef __FUNC_RWLOCK_HPP__
#define __FUNC_RWLOCK_HPP__

#include <pthread.h>

namespace func {

enum RWLockPrefer
{
    RWLockReadPrefer,
    RWLockWritePrefer
};

class RWLock
{
public:
    RWLock(RWLockPrefer mode = RWLockWritePrefer)
    {
        ::pthread_rwlockattr_t attr;
        pthread_rwlockattr_init(&attr);

        if (mode == RWLockWritePrefer)
        {
            pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
        }
        else
        {
            pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_READER_NP);
        }
        pthread_rwlock_init(&mRwLock, &attr);
    }

    ~RWLock()
    {
        pthread_rwlock_destroy(&mRwLock);
    }

public:
    inline void read_lock()
    {
        pthread_rwlock_rdlock(&mRwLock);
    }

    inline void unlock()
    {
        pthread_rwlock_unlock(&mRwLock);
    }

    inline void write_lock()
    {
        pthread_rwlock_wrlock(&mRwLock);
    }

private:
    ::pthread_rwlock_t mRwLock;
};

enum RWLockMode
{
    RWLOCK_READ,
    RWLOCK_WRITE
};

class RWLockScopedGuard
{
public:
    RWLockScopedGuard(RWLock& lock, const RWLockMode& mode) : mLock(lock)
    {
        if (mode == RWLOCK_READ)
        {
            mLock.read_lock();
        }
        else
        {
            mLock.write_lock();
        }
    }

    ~RWLockScopedGuard()
    {
        mLock.unlock();
    }

private:
    RWLock& mLock;
};

}

#endif
