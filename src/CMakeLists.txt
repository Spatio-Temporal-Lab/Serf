cmake_minimum_required(VERSION 3.15)

project(Serf)

# Set C++ standard version
set(CMAKE_CXX_STANDARD 17)

# -O3 Optimization for release version
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Set parallel compilation level as 4
set(CMAKE_BUILD_PARALLEL_LEVEL 4)

# Scan and collect all source code file
file(GLOB_RECURSE LIB_SRC *.cc)

add_library(serf SHARED ${LIB_SRC})

target_include_directories(serf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
