# CMakeList.txt : CMake project for threedangles, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)

if (WIN32)
    set(LIB_SDL2main SDL2::SDL2main)
endif()

add_executable(threedangles "threedangles.cpp")
target_link_libraries(threedangles PRIVATE SDL2::SDL2 ${LIB_SDL2main})

############################### tests ########################################
if(BUILD_TESTING)
    add_subdirectory("test")
endif()
