# CMakeList.txt : CMake project for threedangles, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.18.1)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

if (WIN32)
    set(LIB_SDL2main SDL2::SDL2main)
endif()

set(engine_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CPUID.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vec4.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Mat4.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vertex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Triangle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Cam.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Quat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Light.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Clipping.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Rasterizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Tex3.cpp
    ### SDL dependencies
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl/Screen_SDL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl/Image_SDL.cpp
)

if (CMAKE_CUDA_COMPILER)
    set(engine_sources
        ${engine_sources}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cuda/GPUInfo.cu
    )
endif()

add_library(engine_obj STATIC
    ${engine_sources}
)
set_target_properties(engine_obj PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

target_include_directories(engine_obj PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CUDAToolkit_INCLUDE_DIRS}
)
target_link_libraries(engine_obj PUBLIC
    SDL2::SDL2 ${LIB_SDL2main}
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
)

##### ---- #####
add_executable(threedangles "threedangles.cpp")
target_include_directories(threedangles PRIVATE
    $<TARGET_PROPERTY:engine_obj,INCLUDE_DIRECTORIES>
)
target_link_libraries(threedangles PRIVATE engine_obj)

############################### tests ########################################
if(BUILD_TESTING)
    add_subdirectory("test")
endif()

if(BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()
