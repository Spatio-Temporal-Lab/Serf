cmake_minimum_required(VERSION 3.15)

project(ALP)

# Set C++ standard version
set(CMAKE_CXX_STANDARD 17)

#-----------------------------------------------------------------------------------------------------------------------
include(FetchContent)
include(CheckCXXCompilerFlag)
include(CMakePrintHelpers)
include(CTest)

# CMAKE_SOURCE_DIR: ----------------------------------------------------------------------------------------------------
add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# Include --------------------------------------------------------------------------------------------------------------
include_directories(include)

# Src: -------------------------------------------------------------------------------------------------------
add_subdirectory(src)
