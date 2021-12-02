# CMakeList.txt : CMake project for threedangles, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)


# Add source to this project's executable.
add_executable (threedangles "threedangles.cpp" "threedangles.h")

# TODO: Add tests and install targets if needed.
