# Setup dependency

include(FetchContent)

message(STATUS "Fetching BatteryEmbed")

set(B_PRODUCTION_MODE ON)

FetchContent_Declare(
    battery-embed
    GIT_REPOSITORY  https://github.com/batterycenter/embed.git
    GIT_TAG         v1.2.19
)
FetchContent_MakeAvailable(battery-embed)

message(STATUS "Fetching of BatteryEmbed done!")