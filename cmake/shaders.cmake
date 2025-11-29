find_program(GLSLANG_VALIDATOR glslangValidator REQUIRED)
function(compile_glsl_to_spirv NAME FILE_PATH OUTPUT_DIR)
    set(SHADER_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH})
    set(SHADER_DIST_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/${FILE_PATH}.spv)
    cmake_path(GET SHADER_DIST_PATH PARENT_PATH SHADER_DIST_DIR)

    add_custom_target(compile_glsl_to_spirv_${NAME} ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_DIST_DIR}
        COMMAND ${CMAKE_COMMAND} -E echo "Making directory for ${NAME} at ${SHADER_DIST_PATH}"
        COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER_SOURCE_PATH} -o ${SHADER_DIST_PATH}
        COMMENT "Compiling ${SHADER_SOURCE_PATH} to ${SHADER_DIST_PATH}" 
    )
endfunction(compile_glsl_to_spirv NAME FILE_PATH OUTPUT_DIR)

function(compile_glsl_dir_to_spirv DIR_PATH OUTPUT_DIR)
    set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${DIR_PATH})
    file(GLOB_RECURSE SHADERS
        "${SHADER_SOURCE_DIR}/*.vert"
        "${SHADER_SOURCE_DIR}/*.frag"
    )

    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        file(RELATIVE_PATH SHADER_REL ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER})
        compile_glsl_to_spirv(${SHADER_NAME} ${SHADER_REL} ${OUTPUT_DIR}/${SHADER_NAME})
    endforeach(SHADER ${SHADERS})
endfunction(compile_glsl_dir_to_spirv DIR_PATH OUTPUT_DIR)
