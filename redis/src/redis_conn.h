/*
 * Created: 2018-08-31 20:14 +0800
 *
 * Modified: 2018-08-31 20:14 +0800
 *
 * Description: redisContext的包装，主要用途是可以自动归还链接给连接池
 *
 * Author: jh
 */
#ifndef __FIRE_REDIS_CONN_SHARD_PTR_U__
#define __FIRE_REDIS_CONN_SHARD_PTR_H__

#include <memory>

#include "hiredis/hiredis.h"

namespace fire {

//cosnt or define
#define REDIS_CONN_CHECK_TIME (5) //5s检测一次

//前置声明
class RedisPool;

//RedisConnSharedPtr，类似自动释放指针，这里不是释放而是归还
class RedisConnSharedPtr
{
public:
    RedisConnSharedPtr(redisContext* conext, RedisPool* redis_pool){
        redis_context_p_ = conext;
        belong_pool_p_ = redis_pool;
        last_check_time_ = time(NULL);
    };

    ~RedisConnSharedPtr()
    {
        if (belong_pool_p_ != nullptr) {
            belong_pool_p_->get_back(*this);
        }
    }

    //判断是否需要检测，每次获取链接都应该有此操作
    inline bool need_check(uint64_t now = time(NULL)) const
    {
       if (now - last_check_time_ > REDIS_CONN_CHECK_TIME) {
           return true;
       }

       return false;
    }

private:
    redisContext* redis_context_p_; //redis context
    RedisPool* belong_pool_p_; //自己所属的连接池，不应该有变动
    uint64_t last_check_time_; //上次检测时间，超时后就要ping检查链接
};


}

#endif
