list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/easyexif/exif.cpp )
add_subdirectory( pugixml )

if (NOT EMSCRIPTEN)
    set( SDL_STATIC OFF CACHE BOOL "Build the static SDL library" )
    set( SDL_SHARED ON CACHE BOOL "Build the shared SDL library" )
    add_subdirectory( SDL )

    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tinyfiledialogs/tinyfiledialogs.c)

endif ()

list(APPEND SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_sdl_em.cpp 
)


list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/SDL/include)
list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/stb)
list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/easyexif)
list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/span/include/tcb)
list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/glm)
list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/tinyfiledialogs)
list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
list(APPEND INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})

#message(INFO ${SOURCES})
set(SOURCES_THIRD_PARTY ${SOURCES} PARENT_SCOPE)
set(INCLUDES_THIRD_PARTY ${INCLUDES} PARENT_SCOPE)
