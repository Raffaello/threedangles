# CMakeList.txt : CMake project for threedangles, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.17)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)

if (WIN32)
    set(LIB_SDL2main SDL2::SDL2main)
endif()

add_executable(threedangles "threedangles.cpp")
target_sources(threedangles PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CPUID.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vec4.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Mat4.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Triangle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl/Screen_SDL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Cam.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Quat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Light.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Clipping.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Rasterizer.cpp
)
target_include_directories(threedangles PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CUDAToolkit_INCLUDE_DIRS}
)

if (CMAKE_CUDA_COMPILER)
    set_target_properties(threedangles PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
    target_include_directories(threedangles PRIVATE ${CUDAToolkit_INCLUDE_DIRS})
    target_sources(threedangles PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cuda/GPUInfo.cu
    )
endif()

target_link_libraries(threedangles PRIVATE SDL2::SDL2 ${LIB_SDL2main} ${LIB_CUDA})


############################### tests ########################################
if(BUILD_TESTING)
    add_subdirectory("test")
endif()
