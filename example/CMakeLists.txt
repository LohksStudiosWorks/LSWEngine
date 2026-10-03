cmake_minimum_required(VERSION 3.28)

project(lswe-example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(lsw-engine REQUIRED)

add_executable(example WIN32 main.cpp)

get_target_property(INCLUDES lswe::lsw-engine INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "Includes: ${INCLUDES}")

target_link_libraries(example
    PRIVATE
        lswe::lsw-engine
)

#install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Use CMAKE_PREFIX_PATH to point find package to install folder on call!