/*
 * Created: 2018-09-01 14:21 +0800
 *
 * Modified: 2018-09-01 14:21 +0800
 *
 * Description: redis链接池
 *  1.只支持单个链接地址，如果多个，需要自己创建多个RedisPool实例
 *  2.get_conn是加锁操作，如果操作非常频繁，可以考虑使用多个RedisPool实例
 *  3.RedisConnSharedPtr不需要自己归还，超出作用域后自动归还
 *  4.使用者不要关心链接是否有效以及重连
 *
 * Author: jh
 */
#ifndef __FIRE__REDIS_POOL_H__
#define __FIRE__REDIS_POOL_H__

#include <queue>
#include <string>

#include "redis_conn.h"

namespace fire {

class RedisPool
{
    friend calss RedisConnSharedPtr;
public:
    RedisPool(std::string ip, uint16_t port, uint32_t index, uint32_t conn_num) {
        
    }

    //get conn，有就返回，没有且没有达到最大连接数就创建
    const RedisConnSharedPtr get_conn() const;

private:
    //get back conn
    get_back(RedisConnSharedPtr conn);

private:
    //redis info
    std::string ip_;
    uint16_t port_;
    uint32_t index_;
    uint32_t conn_num_;

    std::queue<RedisConnSharedPtr> redis_conn_ptr_; //contex pool
    std::mutex pool_lock_; //contex pool lock

    //std::vector<std::thread> threads_; //线程，


};

}

