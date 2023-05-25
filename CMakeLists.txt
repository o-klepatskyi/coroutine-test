cmake_minimum_required(VERSION 3.23)
project(Coroutine_Test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-DPROJECT_PATH="${CMAKE_SOURCE_DIR}")

add_subdirectory(src)
add_subdirectory(tests)