include(FetchContent)
add_definitions(-DLOG_LEVEL=0)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
enable_testing()
include(GoogleTest)
set(TARGET_NAME modern_cpp_test)
add_executable(${TARGET_NAME} ${TEST_SOURCES})
target_link_libraries(${TARGET_NAME} PRIVATE gtest modern_cpp_examples)

if(MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE /W4)
else()
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

gtest_discover_tests(modern_cpp_test)