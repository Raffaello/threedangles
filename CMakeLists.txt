﻿# CMakeLit.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.18.1)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()


project ("threedangles")

option(BUILD_EXAMPLES "Build Examples" TRUE)
option(DETECT_CUDA "Detect if CUDA available and use it" FALSE)

include(CTest)

if(DETECT_CUDA)
    include(CheckLanguage)
    check_language(CUDA)
    if(CMAKE_CUDA_COMPILER)
        find_package(CUDAToolkit 11)
        if (CUDAToolkit_FOUND)
            message(STATUS "found CUDAToolkit >11")
            enable_language(CUDA)
            #set(LIB_CUDA cudart cudart_static)
            #set(CMAKE_CUDA_STANDARD 10.2)
            #set(CMAKE_CUDA_STANDARD_REQUIRED ON)
            set(CMAKE_CUDA_ARCHITECTURES OFF)
            add_compile_definitions(WITH_CUDA)
        endif()
    else()
      message(STATUS "No CUDA support")
    endif()
endif()

# Include sub-projects.
add_subdirectory ("threedangles")
