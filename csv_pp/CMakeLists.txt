cmake_minimum_required(VERSION 3.14)
project(csv_pp)

add_executable(csv_pp main.c)

if(MSVC)
  target_compile_options(csv_pp PRIVATE /W4 /WX /O2 /std:clatest)
else()
  target_compile_options(csv_pp PRIVATE -Wall -Wextra -Wpedantic -O2 -std=gnu2x)
endif()

find_package(Python3 REQUIRED)

enable_testing()

add_test(test ${Python3_EXECUTABLE} -u ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.py)
