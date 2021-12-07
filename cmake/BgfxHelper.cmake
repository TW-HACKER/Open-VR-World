include( CMakeParseArguments )

# Adds a folder full of shaders for compilation
function(bgfx_add_shaders arg)
    if(ARGC LESS 3)
        message(STATUS "arguments error when calling drogon_create_views")
        return()
    endif()
    file(MAKE_DIRECTORY ${ARGV2})
    file(GLOB_RECURSE SHADER_LIST ${ARGV1}/*.sc)
    set(SHADER_OUTPUT_LIST "")
    foreach(shader_source ${SHADER_LIST})
         file(RELATIVE_PATH
             input_file
             ${CMAKE_CURRENT_SOURCE_DIR}
             ${shader_source})
        get_filename_component( filename "${shader_source}" NAME_WE )
        if(filename STREQUAL "varying")
            continue()
        endif()
        string( SUBSTRING "${filename}" 0 2 TYPE )
        if( "${TYPE}" STREQUAL "fs" )
            set( TYPE "FRAGMENT" )
            set( D3D_PREFIX "ps" )
        elseif( "${TYPE}" STREQUAL "vs" )
            set( TYPE "VERTEX" )
            set( D3D_PREFIX "vs" )
        elseif( "${TYPE}" STREQUAL "cs" )
            set( TYPE "COMPUTE" )
            set( D3D_PREFIX "cs" )
        else()
            set( TYPE "" )
        endif()
        if(TYPE STREQUAL "")
            message(FATAL_ERROR "Shaders filename must have fs_, ps_ or vs_ prefix. Got ${filename}")
        endif()
        set(input_path "${CMAKE_CURRENT_SOURCE_DIR}/${input_file}")
        set(output_path "${CMAKE_CURRENT_BINARY_DIR}/${input_file}.bin")
        set(depfile_path "${CMAKE_CURRENT_BINARY_DIR}/${input_file}.bin.d")
        add_custom_command(
            OUTPUT "${output_path}" 
            COMMAND bgfx-shaderc
            ARGS -f "${input_path}" -o "${output_path}" --type ${TYPE} --depends --platform linux --profile spirv
            COMMENT "Building shader ${input_file}"
            DEPFILE "${output_path}.d"
            VERBATIM
        )
        set(SHADER_OUTPUT_LIST ${SHADER_OUTPUT_LIST} ${output_path})
    endforeach()
    add_custom_target(shader_compile_${ARGV0} ALL DEPENDS ${SHADER_OUTPUT_LIST})
    add_dependencies(${ARGV0} shader_compile_${ARGV0})
endfunction()
