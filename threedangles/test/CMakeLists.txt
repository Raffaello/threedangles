cmake_minimum_required (VERSION 3.8)

find_package(GTest 1.10 CONFIG REQUIRED)
include(GoogleTest)
include(CMakeParseArguments)

macro(macro_test) # ${ARGN} link libraries
    
    set(prefix TEST)
    set(singleValues EXE FILE)
    set(multiValues FILES LINKS)
    
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multiValues}" ${ARGN})
    #message("${EXE} - ${FILE} - ${LINKS}")
    #message("EXE: ${TEST_EXE}")
    #message("FILE: ${TEST_FILE}")
    #message("FILES: ${TEST_FILES}")
    #message("LINKS: ${TEST_LINKS}")
    add_executable (${TEST_EXE} ${TEST_FILE} ${TEST_FILES})
    target_include_directories(${TEST_EXE} PUBLIC 
        ${CMAKE_SOURCE_DIR}/threedangles/src
    )
    target_link_libraries(${TEST_EXE} PRIVATE GTest::gtest GTest::gmock ${TEST_LINKS})
    add_custom_command(TARGET ${TEST_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:${TEST_EXE}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:${TEST_EXE}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:${TEST_EXE}>
    )

    gtest_discover_tests(${TEST_EXE})
endmacro()

macro_test(
        EXE TestVec3d
        FILE "TestVec3d.cpp"
        FILES ${CMAKE_SOURCE_DIR}/threedangles/src/Vec3d.cpp
    )

macro_test(
        EXE TestMat4
        FILE "TestMat4.cpp"
        FILES
            ${CMAKE_SOURCE_DIR}/threedangles/src/Mat4.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Vec3d.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Triangle.cpp
        #LINKS SDL2::SDL2 ${LIB_SDL2main} # remove this dependency from Mat4 class -> Triangle class
    )
