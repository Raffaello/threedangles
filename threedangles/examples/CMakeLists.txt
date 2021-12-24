### Macro ###
macro(macro_example) # ${ARGN} link libraries
    set(prefix EXAMPLE)
    set(singleValues EXE FILE)
    set(multiValues FILES LINKS FIXTURES)

    include(CMakeParseArguments)
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multiValues}" ${ARGN})
    #message("EXE: ${EXAMPLE_EXE}")
    #message("FILE: ${EXAMPLE_FILE}")
    #message("LINKS: ${EXAMPLE_LINKS}")
    
    add_executable (${EXAMPLE_EXE} ${EXAMPLE_FILE})
    target_sources(${EXAMPLE_EXE} PRIVATE ${EXAMPLE_FILES})
    target_include_directories(${EXAMPLE_EXE} PRIVATE ${CMAKE_SOURCE_DIR}/threedangles/src)
    target_link_libraries(${EXAMPLE_EXE} ${EXAMPLE_LINKS})

    set(fixtures ${EXAMPLE_FIXTURES})
    list(LENGTH fixtures num_fixtures)
    unset(fixtures)
    if(num_fixtures EQUAL 0)
        message("${EXAMPLE_EXE}: No fixtures declared")
    else()
        message("${EXAMPLE_EXE} Fixtures: ${EXAMPLE_FIXTURES}")
        foreach(fixture_i ${EXAMPLE_FIXTURES})
            message("copy fixture ${fixture_i}")
            add_custom_command(TARGET ${EXAMPLE_EXE} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${CMAKE_CURRENT_SOURCE_DIR}/../objs/${fixture_i}
                        ${CMAKE_CURRENT_BINARY_DIR}/${fixture_i}
            )
        endforeach()
    endif()
endmacro()

############################## Main Example ##################################
macro_example(
    EXE "vertex-coloring"
    FILE "vertex-coloring.cpp"
    FILES 
        ${CMAKE_SOURCE_DIR}/threedangles/src/CPUID.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Vec4.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Mat4.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Tex3.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Vertex.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Triangle.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Mesh.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Engine.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/sdl/Screen_SDL.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Cam.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Quat.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Light.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Clipping.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Rasterizer.cpp
    LINKS ${LIB_SDL2main} SDL2::SDL2
)

macro_example(
    EXE "the-cube"
    FILE "the-cube.cpp"
    FILES 
        ${CMAKE_SOURCE_DIR}/threedangles/src/CPUID.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Vec4.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Mat4.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Tex3.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Vertex.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Triangle.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Mesh.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Engine.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Cam.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Quat.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Light.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Clipping.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Rasterizer.cpp
        
        ${CMAKE_SOURCE_DIR}/threedangles/src/sdl/Screen_SDL.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/sdl/Image_SDL.cpp

    LINKS ${LIB_SDL2main} SDL2::SDL2 SDL2::SDL2_image
    FIXTURES
        texture_cube.obj
        wood.png
)
