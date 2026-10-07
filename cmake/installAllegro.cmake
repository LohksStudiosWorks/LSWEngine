include(FetchContent)

set(SHARED OFF CACHE BOOL "Build shared libraries" FORCE)

set(WANT_UNSTABLE ON CACHE BOOL "Enable Allegro unstable API" FORCE)
set(WANT_TESTS OFF CACHE BOOL "Build tests" FORCE)
set(WANT_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(WANT_DEMOS OFF CACHE BOOL "Build demos" FORCE)
set(WANT_DOCS OFF CACHE BOOL "Build documentation" FORCE)

message(STATUS "Fetching Allegro 5...")

FetchContent_Declare(
    allegro5
    GIT_REPOSITORY https://github.com/liballeg/allegro5.git
    GIT_TAG        5.2.11.3
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(allegro5)

function(fix_allegro_interface_includes target_name)
    if(TARGET ${target_name})
        get_target_property(inc_dirs ${target_name} INTERFACE_INCLUDE_DIRECTORIES)
        set_property(TARGET ${target_name} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "")
        target_compile_definitions(${target_name} INTERFACE ALLEGRO_STATICLINK)
        
        if(inc_dirs)
            foreach(dir IN LISTS inc_dirs)
                target_include_directories(${target_name} INTERFACE 
                    $<BUILD_INTERFACE:${dir}>
                    $<INSTALL_INTERFACE:include>
                )
            endforeach()
        else()
            target_include_directories(${target_name} INTERFACE 
                $<INSTALL_INTERFACE:include>
            )
        endif()
    endif()
endfunction()

set(ALLEGRO_TARGETS_TO_FIX
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

foreach(tgt IN LISTS ALLEGRO_TARGETS_TO_FIX)
    fix_allegro_interface_includes(${tgt})
endforeach()
