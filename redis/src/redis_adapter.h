/*
 * Created: 2018-09-01 14:30 +0800
 *
 * Modified: 2018-09-01 14:30 +0800
 *
 * Description: redis adapter
 * 1.redis cmd 的操作封装，建议使用，后续命令不断补充
 *
 * Author: jh
 */
#ifndef __REDIS_ADAPTER_H__
#define __REDIS_ADAPTER_H__

namespace fire {

//原始接口，redisCommand，不建议使用，留着保持灵活请，返回结果需要自己释放
redisReply* reids_command(RedisConnSharedPtr conn, const char* format, ...);

//原始接口，redisCommandArgv，批量操作可以使用，批量操作用户需要自己解析返回值，返回结果要自己释放
redisReply* redis_command_argv(RedisConnSharedPtr conn, int argc, const char** argv, const size_t *argvlen);

//原始接口，redisAppendCommand，管道
void redis_append_command(RedisConnSharedPtr conn, const char *format, ...);
void redis_append_command_argv(RedisConnSharedPtr conn, int argc, const char **argv, const size_t *argvlen);

//CONNECT oper
bool redis_ping(RedisConnSharedPtr conn);

//KEY oper
//del
bool redis_del(RedisConnSharedPtr conn, const char* key, std::string* err = nullptr);
//dump
//exists
//expire
//expireat
//keys
//migrate
//move
//object
//persist
//pexpire
//pexpireat
//pttl
//randomkey
//rename
//renamenx
//restore
//sort
//ttl
//type
//scan

//STRING oper
//append

//HASH oper
//hdel

//LIST oper

//SET oper

//SORTEDSEAT oper

//PUB/SUB oper

//TRANSACTION oper

//SERVER oper

}

#endif
