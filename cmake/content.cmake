function(copy_content_directory CONTENT_DIR)
    add_custom_target(copy_content_${CONTENT_DIR} ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/${CONTENT_DIR}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/${CONTENT_DIR}
    )
endfunction(copy_content_directory CONTENT_DIR)
