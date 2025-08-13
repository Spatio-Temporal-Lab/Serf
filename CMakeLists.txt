cmake_minimum_required(VERSION 3.15)

project(Serf-All)

# Print system info
if(WIN32)
    message(STATUS "[SelfCheck] Host System: Windows (32bit)")
    add_definitions(-DWIN32)
elseif(WIN64)
    message(STATUS "[SelfCheck] Host System: Windows (64bit)")
    add_definitions(-DWIN64)
elseif(APPLE)
    message(STATUS "[SelfCheck] Host System: Apple MacOS")
    add_definitions(-D__MACH__)
elseif(UNIX)
    message(STATUS "[SelfCheck] Host System: Linux/Unix/*nix")
    add_definitions(-DUNIX -DLINUX)
else()
    message(STATUS "[SelfCheck] Host System: Unknown")
endif(WIN32)

# Serf library
add_subdirectory(src)

# To run perf programs and unit tests
add_subdirectory(test)

# Serf command line interface
add_subdirectory(serf_cmd)

# Uncomment this line to use Python interface of Serf
# add_subdirectory(pywrapper)
