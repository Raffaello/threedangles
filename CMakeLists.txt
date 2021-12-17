# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.18.1)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()


project ("threedangles")
include(CTest)

include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
  enable_language(CUDA)
  find_package(CUDAToolkit REQUIRED 11)
  set(LIB_CUDA cudart cudart_static)
  #set(CMAKE_CUDA_STANDARD 10.2)
  #set(CMAKE_CUDA_STANDARD_REQUIRED ON)
  set(CMAKE_CUDA_ARCHITECTURES OFF)
  add_compile_definitions(WITH_CUDA)
else()
  message(STATUS "No CUDA support")
endif()

# Include sub-projects.
add_subdirectory ("threedangles")
