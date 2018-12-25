list(APPEND SOURCES2 ${CMAKE_CURRENT_SOURCE_DIR}/easyexif/exif.cpp )

if (NOT EMSCRIPTEN)
    set( SDL_STATIC OFF CACHE BOOL "Build the static SDL library" )
    set( SDL_SHARED ON CACHE BOOL "Build the shared SDL library" )
    add_subdirectory( SDL )

    list(APPEND SOURCES2 ${CMAKE_CURRENT_SOURCE_DIR}/tinyfiledialogs/tinyfiledialogs.c)

endif ()

add_subdirectory( pugixml )
#message(INFO ${SOURCES2})
set(SOURCES_THIRD_PARTY ${SOURCES2} PARENT_SCOPE)
