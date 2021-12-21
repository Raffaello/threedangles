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
        EXE TestVec4
        FILE "TestVec4.cpp"
        FILES ${CMAKE_SOURCE_DIR}/threedangles/src/Vec4.cpp
    )

macro_test(
        EXE TestMat4
        FILE "TestMat4.cpp"
        FILES
            ${CMAKE_SOURCE_DIR}/threedangles/src/Mat4.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Vec4.cpp
    )

macro_test(
    EXE TestQuat
    FILE "TestQuat.cpp"
    FILES ${CMAKE_SOURCE_DIR}/threedangles/src/Quat.cpp
)

macro_test(
        EXE TestColor
        FILE "TestColor.cpp"
        FILES
    )

macro_test(
        EXE TestVertex
        FILE "TestVertex.cpp"
        FILES
            ${CMAKE_SOURCE_DIR}/threedangles/src/Vec4.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Vertex.cpp
)


macro_test(
        EXE TestTriangle
        FILE "TestTriangle.cpp"
        FILES
            ${CMAKE_SOURCE_DIR}/threedangles/src/Mat4.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Vec4.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Vertex.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Triangle.cpp
)


macro_test(
        EXE TestEngine
        FILE "TestEngine.cpp"
        FILES
            ${CMAKE_SOURCE_DIR}/threedangles/src/Mat4.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Vec4.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Triangle.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Mesh.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Cam.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Light.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Clipping.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Engine.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/Rasterizer.cpp
            ${CMAKE_SOURCE_DIR}/threedangles/src/sdl/Screen_SDL.cpp # @todo remove
        LINKS SDL2::SDL2 ${SDL2Main} # This should be removed and use a Screen Stub
    )
