#pragma once

#include <string>
#include <cstdint>
#include <chrono>

inline std::string getProjectPath() noexcept
{
    return PROJECT_PATH;
}

template <typename Duration, typename T>
std::uint64_t elapsedTime(T startTime) noexcept
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<Duration>(currentTime - startTime);
    return elapsed.count();
};

template<typename T>
auto elapsedTimeMs(T startTime) noexcept
{
    return elapsedTime<std::chrono::milliseconds>(startTime);
};

template<typename T>
auto elapsedTimeNano(T startTime) noexcept
{
    return elapsedTime<std::chrono::nanoseconds>(startTime);
};