cmake_minimum_required(VERSION 3.14)
project(aerosoft)

add_executable(aerosoft main.c)

if(MSVC)
  target_compile_options(aerosoft PRIVATE /W4 /WX /O2 /std:clatest)
else()
  target_compile_options(aerosoft PRIVATE -Wall -Wextra -Wpedantic -O2 -std=gnu2x)
endif()
