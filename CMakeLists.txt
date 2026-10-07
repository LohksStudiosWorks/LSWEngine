cmake_minimum_required(VERSION 3.28)

project(lsw-engine
    DESCRIPTION "A simple memory library"
    VERSION "6"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PROJECT_SOURCES
     CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

message(STATUS "Files used for compilation of lswe::${PROJECT_NAME}:")
foreach(file IN LISTS PROJECT_SOURCES)
    message(STATUS " - ${file}")
endforeach()


add_library(${PROJECT_NAME})
add_library(lswe::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(cmake/installBatteryEmbed.cmake)
include(cmake/installAllegro.cmake)
include(cmake/MinGWFix.cmake)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        ALLEGRO_UNSTABLE
        ALLEGRO_STATICLINK
    PRIVATE
        $<$<CONFIG:Debug>:LSWE_DEBUG>
        $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0A00>
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    allegro
    allegro_main
    allegro_image
    allegro_font
    allegro_ttf
    allegro_audio
    allegro_acodec
    allegro_primitives
    allegro_color
    allegro_dialog
    allegro_video
    allegro_memfile
    allegro_physfs
)


option(BUILD_TESTS "Build tests" ON)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    if (BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()

    include(cmake/install.cmake)
endif()

#install(TARGETS ${PROJECT_NAME} DESTINATION bin)