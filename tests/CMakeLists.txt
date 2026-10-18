enable_testing()

# Dumb VS Code warning fix
file(WRITE ${CMAKE_BINARY_DIR}/DartConfiguration.tcl "")

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS src/*.cpp)

if(WIN32)
    set(TARGET_PLATFORM "Windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(TARGET_PLATFORM "Linux")
else()
    set(TARGET_PLATFORM ${CMAKE_SYSTEM_NAME})
endif()

set(OGG_TEST_LIST
    test_audio_music_playback
)

set(JPG_TEST_LIST
    test_display_works
    test_locked_region_cyberpunk
)

set(PNG_TEST_LIST
    test_vertex_transform
)

set(GIF_TEST_LIST
    test_gif_works
)

set(FONT_TEST_LIST
    test_font_works
)

set(VIDEO_TEST_LIST
    test_video_works
)


foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)

    message(STATUS "Building ${test_name}")

    add_executable(${test_name} ${test_src})

    target_link_libraries(${test_name} PRIVATE lswe::lsw-engine)
    
    target_compile_definitions(${test_name} PRIVATE LSWE_DEBUG)
    
    if(test_name IN_LIST OGG_TEST_LIST)
        b_embed(${test_name} "resources/7. Continuity.ogg")
    endif()

    if(test_name IN_LIST JPG_TEST_LIST)
        b_embed(${test_name} "resources/vrchat_ex.jpg")
    endif()

    if(test_name IN_LIST PNG_TEST_LIST)
        b_embed(${test_name} "resources/dirt_block.png")
    endif()

    if(test_name IN_LIST GIF_TEST_LIST)
        b_embed(${test_name} "resources/my_gif.gif")
    endif()

    if(test_name IN_LIST VIDEO_TEST_LIST)
        b_embed(${test_name} "resources/video.ogv")
    endif()

    if(test_name IN_LIST FONT_TEST_LIST)
        b_embed(${test_name} "resources/calibri_fix.ttf")
    endif()

    add_test(NAME ${test_name} COMMAND ${test_name} ${TARGET_PLATFORM})
endforeach()