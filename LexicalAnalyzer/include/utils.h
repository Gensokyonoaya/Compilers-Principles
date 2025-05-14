#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <algorithm>
#include <vector>
#include <set>

// 日志开关：设置为 1 启用日志，设置为 0 禁用日志
#define ENABLE_LOG 1

#if ENABLE_LOG
    #define LOG(level, message) \
        std::cerr << "[" << level << "] " << message \
                  << " (File: " << __FILE__ \
                  << ", Line: " << __LINE__ << ")" << std::endl;

    #define DEBUG_ERROR(message) LOG("ERROR", message)
    #define DEBUG_WARNING(message) LOG("WARNING", message)
    #define DEBUG_INFO(message) LOG("INFO", message)
#else
    #define LOG(level, message) // 空实现，禁用日志
    #define DEBUG_ERROR(message) // 空实现，禁用错误日志
    #define DEBUG_WARNING(message) // 空实现，禁用警告日志
    #define DEBUG_INFO(message) // 空实现，禁用信息日志
#endif

void trim(std::string& str);
std::vector<std::string> splitIntoSymbols(const std::string& segment);


#endif // UTILS_H