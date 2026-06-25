cmake_minimum_required(VERSION 3.15)
project(wh0revshell CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(wh0revshell "src/wh0revshell.cpp")

# Explicitly link Windows socket library
target_link_libraries(wh0revshell PRIVATE ws2_32)

# Force Static Linking
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
