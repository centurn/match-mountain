cmake_minimum_required(VERSION 3.5.1)

project (match-mountain)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(third-party)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/SDL/include)

add_executable(match-mountain match-mountain/src/main.cpp)
target_link_libraries(match-mountain SDL2)
