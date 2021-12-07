cmake_minimum_required(VERSION 3.21)
project(open-vr-world)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)

include(BgfxHelper)

add_subdirectory(open-vr-world)
