#include <cstdio>
#include <iostream>
#include <thread>

#define LOG_LEVEL_NO_LOG    0
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_INFO      2

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LogInfo(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
#define LogInfo(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LogError(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else 
#define LogError(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LogThreadId() std::cout << __FUNCTION__ << ":" << __LINE__ << " thread id: " << std::this_thread::get_id() << "\n" 
#else
#define LogThreadId()
#endif