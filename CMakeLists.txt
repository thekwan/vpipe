cmake_minimum_required(VERSION 2.8)
project(vpipe)

find_package(OpenCV REQUIRED)

#set(CMAKE_CXX_COMPILE_FLAGS "-std=c++0x -O3")
set(CMAKE_CXX_FLAGS "-std=c++14 -g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories(
    "inc"
    "inc/feature"
    "inc/misc")

FILE(GLOB_RECURSE sourcefiles "src/*.cc")

add_executable(vpipe ${sourcefiles} main.cc)
target_link_libraries(vpipe ${OpenCV_LIBS})
#add_executable(vpipe_test ${sourcefiles} )
#target_link_libraries(vpipe_test ${OpenCV_LIBS})
