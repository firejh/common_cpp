#include "config.h"

Config::Config()
{
    ini_ = NULL;
}

Config::~Config()
{
    if (NULL != ini_) {
        iniparser_freedict(ini_);
    }
}

void Config::load(const char* path)
{
    ini_ = iniparser_load(path);
    if (NULL == ini_) {
        //throw;
        return;
    }

    process_name_ = iniparser_getstring(ini_, "server:process_name", "meta_server");

    log_sys_file_ = iniparser_getstring(ini_, "spdlog:sys_file", "/data/logs/sys_file");
    log_logic_file_ = iniparser_getstring(ini_, "spdlog:logic_file", "/data/logs/logic_file");
    log_sys_level_ = iniparser_getint(ini_, "spdlog:sys_level", 0);
    log_logic_level_ = iniparser_getint(ini_, "spdlog:logic_level", 0);
    log_max_size_ =  iniparser_getint(ini_, "spdlog:log_file_size", 1000);//M

    return;
}
