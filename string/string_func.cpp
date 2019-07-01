#include "string.h"

#include "string_func.h"

namespace func {

bool str_is_num(const std::string& str)
{
    if (str.empty()) {
        return false;
    }   
    for (size_t i = 0; i < str.size(); i++) {
        if (tmp >= 48 && tmp <= 57) {
            continue;
        } else {
            return false;
        }   
    }   
    return true;
}

uint64_t str_to_ull(const char* str)
{
    /* 必须保证str是数字字符串 */
    return strtoull(str, NULL, 10);
}

uint32_t str_to_uint(const char* str)
{
    /* 必须保证str是数字字符串 */
    return strtoul(str, NULL, 10);
}

void split(std::vector<std::string>& result, std::string input, const char* regex)
{
    if (input.size() <= 0) return;

    int pos = 0;  
    int npos = 0;  
    int regexlen = strlen(regex);  
    while ((npos=input.find(regex, pos)) != -1) {   
        std::string tmp = input.substr(pos, npos-pos);  
        result.push_back(tmp);  
        pos = npos+regexlen;  
    }   
    result.push_back(input.substr(pos, input.length() - pos));
}

void split(std::vector<uint64_t>& result, std::string input, const char* regex)
{
    if (input.size() <= 0) return;

    int pos = 0;  
    int npos = 0;  
    int regexlen = strlen(regex);  
    while ((npos=input.find(regex, pos)) != -1) {   
        std::string tmp = input.substr(pos,npos-pos);  
        if (!str_is_num(tmp)) {
            return;
        }   
        result.push_back(strtoull(tmp.c_str(), NULL, 10));  
        pos = npos+regexlen;  
    }   
    std::string tmp = input.substr(pos, input.length()-pos);
    if (!str_is_num(tmp)) {
        return;
    }
    result.push_back(strtoull(tmp.c_str(), NULL, 10));
}

void str_to_lower(char* strs)
{
    int len = strlen(strs);
    for(int i = 0; i < len; i ++)
    {
        if(strs[i] >= 'A' && strs[i] <= 'Z')
            strs[i] += 32; 
    }
}

bool str_is_ip(const std::string& str)
{
    std::vector<uint64_t> ip_arry;
    split(ip_arry, str, ".");
    if(ip_arry.size() != 4)
    {
        return false;
    }

    for(size_t i = 0; i < ip_arry.size(); ++i)
    {
        if(ip_arry[i] > 255)
        {
            return false;
        }
    }

    return true;
}

bool str_is_addr_list(const std::string& str)
{
    std::vector<std::string> addr_str;
    split(addr_str, str, ",");
    
    if(addr_str.empty())
    {
        return false;
    }

    for(size_t i = 0; i < addr_str.size(); ++i)
    {
        std::vector<std::string> addr_ip_port;
        split(addr_ip_port, addr_str[i], ":");
        if(addr_ip_port.size() != 2)
        {
            return false;
        }
        if(!str_is_ip(addr_ip_port[0]) || !str_is_num(addr_ip_port[1]))
        {
            return false;
        }

        uint32_t port = str_to_uint(addr_ip_port[1].c_str());
        if(port <= 1024 || port >= 65534)
        {
            return false;
        }
    }

    return true;
}

}
