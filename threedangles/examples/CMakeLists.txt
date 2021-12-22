### Macro ###
macro(macro_example) # ${ARGN} link libraries
    set(prefix EXAMPLE)
    set(singleValues EXE FILE)
    set(multiValues FILES LINKS)

    include(CMakeParseArguments)
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multiValues}" ${ARGN})
    #message("EXE: ${EXAMPLE_EXE}")
    #message("FILE: ${EXAMPLE_FILE}")
    #message("LINKS: ${EXAMPLE_LINKS}")
    
    add_executable (${EXAMPLE_EXE} ${EXAMPLE_FILE})
    target_sources(${EXAMPLE_EXE} PRIVATE ${EXAMPLE_FILES})
    target_include_directories(${EXAMPLE_EXE} PRIVATE ${CMAKE_SOURCE_DIR}/threedangles/src)
    target_link_libraries(${EXAMPLE_EXE} ${EXAMPLE_LINKS})
endmacro()

############################## Main Example ##################################
macro_example(
    EXE "vertex-coloring"
    FILE "vertex-coloring.cpp"
    FILES 
        ${CMAKE_SOURCE_DIR}/threedangles/src/CPUID.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Vec4.cpp
        ${CMAKE_SOURCE_DIR}/threedangles/src/Mat4.cpp
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
