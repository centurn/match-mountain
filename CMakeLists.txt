cmake_minimum_required(VERSION 3.5.1)

project (match-mountain)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(match-mountain/src)

find_package(OpenGL REQUIRED)
set(LIBS
  ${OPENGL_LIBRARY}
  pugixml
)

if (EMSCRIPTEN)
    add_definitions( -DPICS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/test/" )
    add_definitions( -DSRTM_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/test/" )
else ()
    add_definitions( -DPICS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../test_pictures/" )
    add_definitions( -DSRTM_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../SRTM/" )
endif ()

option(ASG_PROFILE "Build-in high-level profiling" OFF)
if(ASG_PROFILE)
    add_definitions(-DASG_PROFILE)
endif()

add_subdirectory(third-party)

if (EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "--closure 1 -fno-rtti -s USE_SDL=2 -s TOTAL_MEMORY=157286400 -s DISABLE_EXCEPTION_CATCHING=0 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets/test/")
else ()
  set(LIBS
    SDL2
    ${LIBS}
  )

endif()

add_executable(match-mountain ${SOURCES} ${SOURCES_THIRD_PARTY})
target_include_directories(match-mountain PUBLIC ${INCLUDES_THIRD_PARTY} ${CMAKE_CURRENT_SOURCE_DIR}/match-mountain/src )

#message(STATUS,${SOURCES_THIRD_PARTY})
target_link_libraries(match-mountain ${LIBS})
