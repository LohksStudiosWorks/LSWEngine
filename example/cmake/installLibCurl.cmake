include(FetchContent)

set(BUILD_CURL_EXE OFF CACHE BOOL "Disable curl executable" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Disable curl tests" FORCE)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static curl" FORCE)

FetchContent_Declare(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG        curl-8_22_0
)

FetchContent_MakeAvailable(curl)