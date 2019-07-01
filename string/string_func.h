/*
 * Created: 2019-03-08 19:42 +0800
 *
 * Modified: 2019-03-08 19:42 +0800
 *
 * Description: 字符串的操作函数
 *
 * Author: jh
 */
#ifndef __STRING_FUNC_H__
#define __STRING_FUNC_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>

namespace func {

/* 判断是否为数字 */
bool str_is_num(const std::string& str);

/* 转为uint64_t */
uint64_t str_to_ull(const char* str);

uint32_t str_to_uint(const char* str);

/* 拆分string */
void split(std::vector<std::string>& result, std::string input, const char* regex);

/* 拆分string为数值 */
void split(std::vector<uint64_t>& result, std::string input, const char* regex);

/* 将输入的字符串转换为小写字符 */
void str_to_lower(char* strs);

/* 是否是ip*/
bool str_is_ip(const std::string& str);

/* 是否是 ip list，格式为 "127.0.0.1:8000,127.0.0.1:8001" */
bool str_is_addr_list(const std::string& str);

}

#endif//__STRING_FUNC_H__
