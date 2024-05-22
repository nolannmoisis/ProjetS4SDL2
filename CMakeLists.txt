cmake_minimum_required(VERSION 3.22)
project(ProjetS2-SDL)

set(CMAKE_C_STANDARD 99)

add_executable(${PROJECT_NAME} main.c C/Graph.c
        C/ListInt.c
        C/MapPtr.c
        C/MaxFlow.c
        C/ShortestPath.c
        C/TreeInt.c
        C/GeoJson.c
        Header/GeoJson.h
        C/TSP.c
        C/Destination.c)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(SDL2_PATH "./SDL2/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})