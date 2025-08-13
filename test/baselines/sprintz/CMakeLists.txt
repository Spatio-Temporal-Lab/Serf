cmake_minimum_required(VERSION 3.15)

project(Sprintz)

# Set C++ standard version
set(CMAKE_CXX_STANDARD 11)

# -O3 Optimization for release version
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Set parallel compilation level as 4
set(CMAKE_BUILD_PARALLEL_LEVEL 4)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cpp *.cc)

add_library(sprintz STATIC ${LIB_SRC})

target_compile_options(sprintz PRIVATE -march=native -mavx2 -mbmi -mbmi2)
