cmake_minimum_required(VERSION 3.20)
project(Cocoa)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

include(cmake/shaders.cmake)

find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

find_package(SDL3 CONFIG REQUIRED)

add_executable(Cocoa
    src/main.cpp
    
    src/vulkan/device.cpp
    src/vulkan/encoder.cpp
    src/vulkan/surface.cpp
    src/vulkan/swapchain.cpp
    src/vulkan/vma.cpp

    src/vulkan/resources/buffer.cpp
    src/vulkan/resources/sampler.cpp
    src/vulkan/resources/shader_module.cpp
    src/vulkan/resources/render_pipeline.cpp
    src/vulkan/resources/pipeline_layout.cpp
    src/vulkan/resources/texture.cpp
    src/vulkan/resources/bind_group.cpp
)

target_link_libraries(Cocoa
    PRIVATE Vulkan::Vulkan
    PRIVATE GPUOpen::VulkanMemoryAllocator
    PRIVATE SDL3::SDL3
)

if(APPLE)
    target_link_libraries(Cocoa
        PRIVATE "-framework AppKit"
    )
endif()

compile_glsl_dir_to_spirv(content content)

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