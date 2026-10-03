enable_testing()

# Dumb VS Code warning fix
file(WRITE ${CMAKE_BINARY_DIR}/DartConfiguration.tcl "")

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS src/*.cpp)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)

    add_executable(${test_name} ${test_src})

    target_link_libraries(${test_name} PRIVATE lswe::lsw-engine)
    
    target_compile_definitions(${test_name} PRIVATE LUNARIS_DEBUG_FLAG_TEST)

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()