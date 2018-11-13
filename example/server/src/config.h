#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>
#include <string>

#include "iniparser/iniparser.h"

class Config
{
public:
    Config();
    ~Config();

    void load(const char* path);

    std::string get_process_name() {
        return process_name_;
    }
    std::string get_sys_log_file() {
        return log_sys_file_;
    }
    std::string get_logic_log_file() {
        return log_logic_file_;
    }
    int32_t get_sys_log_level() {
        return log_sys_level_;
    }
    int32_t get_logic_log_level() {
        return log_logic_level_;
    }
    int32_t get_log_max_size() {
        return log_max_size_;
    }

private:
    dictionary* ini_;

    std::string process_name_;      //进程名，一台机器部署多个需要区分名称

    std::string log_sys_file_;      //日志路径+日志名
    std::string log_logic_file_;    //日志路径+日志名
    int32_t log_sys_level_;         //日志等级
    int32_t log_logic_level_;       //日志等级
    int32_t log_max_size_;          //日志文件大小

};

#endif
