file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)
add_definitions(-DLOG_LEVEL=2)
add_library(coroutine_test_lib ${SOURCES})
add_executable(coroutine_test_run ${SOURCES})
target_include_directories(coroutine_test_lib PUBLIC include)
target_include_directories(coroutine_test_run PUBLIC include)