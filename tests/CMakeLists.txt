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

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)

    message(STATUS "Building ${test_name}")

    add_executable(${test_name} ${test_src})

    target_link_libraries(${test_name} PRIVATE lswe::lsw-engine)
    
    target_compile_definitions(${test_name} PRIVATE LSWE_DEBUG)

    b_embed(${test_name} "resources/7. Continuity.ogg")
    b_embed(${test_name} "resources/vrchat_ex.jpg")

    add_test(NAME ${test_name} COMMAND ${test_name} ${TARGET_PLATFORM})
endforeach()