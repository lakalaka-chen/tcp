cmake_minimum_required(VERSION 3.20)
project(tcp VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Debug")

enable_testing()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/third_party/spdlog/include
        ${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include)

include_directories(${INCLUDE_DIR})
include_directories(BEFORE src)


set(TCP_INCLUDE_FILES
        include/tcp_client.h
        include/tcp_server.h
        include/tcp_socket.h)
set(TCP_SOURCE_FILES
        src/tcp_server.cpp
        src/tcp_client.cpp
        src/tcp_socket.cpp)

add_library(tcp
        ${TCP_INCLUDE_FILES}
        ${TCP_SOURCE_FILES})

add_subdirectory(tests)
add_subdirectory(third_party)