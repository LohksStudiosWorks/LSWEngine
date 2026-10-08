cmake_minimum_required(VERSION 3.28)

project(lswe-example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include (cmake/installLibCurl.cmake)
include (cmake/installJson.cmake)

find_package(lsw-engine REQUIRED)

add_executable(example main.cpp) # put WIN32 to avoid terminal on Windows, if desired as such

get_target_property(INCLUDES lswe::lsw-engine INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "Includes: ${INCLUDES}")

target_link_libraries(example
    PRIVATE
        lswe::lsw-engine
        nlohmann_json::nlohmann_json
        libcurl
)

#install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Use CMAKE_PREFIX_PATH to point find package to install folder on call!