cmake_minimum_required(VERSION 3.20)
project(Cocoa)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

#include(cmake/shaders.cmake)
#include(cmake/content.cmake)

find_package(SDL3 CONFIG REQUIRED)
find_package(Stb REQUIRED)

find_library(DISCORD_RPC_LIB 
    NAMES discord-rpc libdiscord-rpc
    REQUIRED
)

add_executable(Cocoa
    src/main.cpp

    src/tools/rich_presence.cpp
    src/tools/stb.h
    src/tools/stb.cpp
)

target_include_directories(Cocoa
    PRIVATE ${Stb_INCLUDE_DIR}
)

target_link_libraries(Cocoa
    PRIVATE SDL3::SDL3
    PRIVATE ${DISCORD_RPC_LIB}
)

if(APPLE)
    target_link_libraries(Cocoa
        PRIVATE "-framework AppKit"
    )
elseif(WIN32)
    target_link_options(Cocoa 
        PRIVATE $<$<CONFIG:Release>:-mwindows>
    )
endif()

#compile_glsl_dir_to_spirv(shaders shaders)
#copy_content_directory(content)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(Copy_Compiled_Commands ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_BINARY_DIR}/../compile_commands.json
        COMMENT "Moving compiled commands from ${CMAKE_CURRENT_BINARY_DIR} to ${CMAKE_CURRENT_BINARY_DIR}/../"
    )
endif()

if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    get_filename_component(COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    add_custom_command(TARGET Cocoa POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COMPILER_DIR}/../bin/libstdc++-6.dll"
            "$<TARGET_FILE_DIR:Cocoa>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COMPILER_DIR}/../bin/libgcc_s_seh-1.dll"
            "$<TARGET_FILE_DIR:Cocoa>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COMPILER_DIR}/../bin/libwinpthread-1.dll"
            "$<TARGET_FILE_DIR:Cocoa>"
        COMMENT "Copying MinGW runtime DLLs"
    )
endif()