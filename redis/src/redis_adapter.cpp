#include "redis_adapter.h"

namespace fire {

redisReply* reids_command(RedisConnSharedPtr conn, const char* format, ...)
{
}

void redis_append_command(RedisConnSharedPtr conn, const char *format, ...)
{
}

void redis_append_command_argv(RedisConnSharedPtr conn, int argc, const char ** argv, const size_t *argvlen)
{
}

bool redis_ping(RedisConnSharedPtr conn)
{
}

bool redis_del(RedisConnSharedPtr conn, const char* key, std::string* err = nullptr)
{
}

}
