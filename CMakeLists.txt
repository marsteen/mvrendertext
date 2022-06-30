cmake_minimum_required(VERSION 3.10)
project(mvrendertext)

include_directories(./source)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

add_executable(${CMAKE_PROJECT_NAME}
    ./source/main.cpp
    ./source/Px437_ATI_8x14.cpp
    ./source/mvpng/src/mv_graphics_png.c
)

target_link_libraries(${CMAKE_PROJECT_NAME}
        png
)


