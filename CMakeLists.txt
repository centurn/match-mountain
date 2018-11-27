cmake_minimum_required(VERSION 3.5.1)

project (match-mountain)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(match-mountain/src)

find_package(OpenGL REQUIRED)
set(LIBS
  ${OPENGL_LIBRARY}
)

add_definitions( -DASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/test/" )
IF (EMSCRIPTEN)
  SET(CMAKE_CXX_FLAGS "-s USE_SDL=2 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets/test/")
ELSE ()
  add_subdirectory(third-party)
  set(LIBS
    SDL2
    ${LIBS}
  )

ENDIF()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/SDL/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/stb)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/span/include/tcb)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../span/include/tcb)


#message(STATUS, ${SOURCES})
add_executable(match-mountain ${SOURCES})
target_link_libraries(match-mountain ${LIBS})
