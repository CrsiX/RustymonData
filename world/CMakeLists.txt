cmake_minimum_required(VERSION 3.14)
project(world_generator)

set(CMAKE_CXX_STANDARD 14)
set(BUILD_SHARED_LIBS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Osmium REQUIRED COMPONENT io)

if(NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif()
include_directories(${OSMIUM_INCLUDE_DIRS})

include(FetchContent)
FetchContent_Declare(
        cpr GIT_REPOSITORY ${PROJECT_SOURCE_DIR}/libcpr  # https://github.com/libcpr/cpr
        GIT_TAG 21f42cf882d0b7e5ae9e3434574fc47e187728de)
FetchContent_MakeAvailable(cpr)

add_executable(world_generator main.cpp exporter.cpp generator.cpp)
target_link_libraries(world_generator
        PRIVATE cpr::cpr
        pthread
        z
        expat
        bz2
        jsoncpp)
