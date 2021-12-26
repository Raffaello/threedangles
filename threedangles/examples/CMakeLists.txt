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
    LINKS $<TARGET_OBJECTS:engine_obj> $<TARGET_PROPERTY:engine_obj,LINK_LIBRARIES>
)

macro_example(
    EXE "the-cube"
    FILE "the-cube.cpp"
    LINKS $<TARGET_OBJECTS:engine_obj> $<TARGET_PROPERTY:engine_obj,LINK_LIBRARIES>
    FIXTURES
        texture_cube.obj
        wood.png
)
