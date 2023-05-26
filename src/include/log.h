#pragma once
#include <cstdio>
#include <iostream>
#include <thread>
#include <mutex>

static std::mutex logging_mutex;

#define LOG_LEVEL_NO_LOG    0
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_INFO      2

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LogInfo(fmt, ...) { std::lock_guard<std::mutex> lg(logging_mutex); std::cout << "[" << std::this_thread::get_id() << "] "; printf(fmt "\n", ##__VA_ARGS__); }
#else
#define LogInfo(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LogError(fmt, ...) { std::lock_guard<std::mutex> lg(logging_mutex); std::cout << "[" << std::this_thread::get_id() << "] "; fprintf(stderr, fmt "\n", ##__VA_ARGS__); }
#else 
#define LogError(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LogThreadId() { std::lock_guard<std::mutex> lg(logging_mutex); std::cout << __FUNCTION__ << ":" << __LINE__ << " thread id: " << std::this_thread::get_id() << "\n"; }
#else
#define LogThreadId()
#endif