#pragma once

#include <string>
#include <cstdint>
#include <chrono>

std::string getProjectPath() noexcept
{
    return PROJECT_PATH;
}

template <typename Duration, typename T>
inline std::uint64_t elapsedTime(T startTime) noexcept
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<Duration>(currentTime - startTime);
    return elapsed.count();
};

template<typename T>
inline auto elapsedTimeMs(T startTime) noexcept
{
    return elapsedTime<std::chrono::milliseconds>(startTime);
};

template<typename T>
inline auto elapsedTimeNano(T startTime) noexcept
{
    return elapsedTime<std::chrono::nanoseconds>(startTime);
};