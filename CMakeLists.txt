cmake_minimum_required(VERSION 3.20)
project(Cocoa)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
    GIT_TAG release-3.2.26
)
FetchContent_MakeAvailable(SDL3)

find_package(Vulkan REQUIRED)

add_executable(Cocoa
    main.cpp
)

target_link_libraries(Cocoa
    PRIVATE Vulkan::Vulkan
    PRIVATE SDL3::SDL3
)