cmake_minimum_required(VERSION 3.20)
project(wh0loader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(wh0loader
    src/wh0loader.cpp
)

# Windows libs
if(WIN32)
    target_link_libraries(wh0loader PRIVATE ws2_32)
endif()

# Static runtime (GCC MinGW)
if(MINGW)
    target_link_options(wh0loader PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
        -pthread
    )
endif()
