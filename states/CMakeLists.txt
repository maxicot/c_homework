cmake_minimum_required(VERSION 3.14)
project(states)

add_executable(states main.c)

if(MSVC)
  target_compile_options(states PRIVATE /W4 /WX /O2 /std:clatest)
else()
  target_compile_options(states PRIVATE -Wall -Wextra -Wpedantic -O2 -std=gnu2x)
endif()
