/*
 * Created: 2019-05-23 17:17 +0800
 *
 * Modified: 2019-05-23 17:17 +0800
 *
 * Description: lock guard
 *
 * Author: jh
 */
#ifndef __FUNC_MUTEX_LOCK_HPP__
#define __FUNC_MUTEX_LOCK_HPP__

#include <pthread.h>

namespace func {

class Lock
{
public:
    Lock()
    {
        pthread_mutex_init(&lock_, NULL);
    }

    ~Lock()
    {
        pthread_mutex_destroy(&lock_);
    }

public:
    inline void lock()
    {
        pthread_mutex_lock(&lock_);
    }

    inline void unlock()
    {
        pthread_mutex_unlock(&lock_);
    }

private:
    ::pthread_mutex_t lock_;
};

class LockScopedGuard
{
public:
    LockScopedGuard(Lock& lock) :lock_(lock)
    {
        lock_.lock();
    }

    ~LockScopedGuard()
    {
        lock_.unlock();
    }

private:
    Lock& lock_;
};

}

#endif
