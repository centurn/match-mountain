if (NOT EMSCRIPTEN)
    set( SDL_STATIC OFF CACHE BOOL "Build the static SDL library" )
    set( SDL_SHARED ON CACHE BOOL "Build the shared SDL library" )
    add_subdirectory( SDL )
endif ()

set(SOURCES_THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/easyexif/exif.cpp PARENT_SCOPE)

