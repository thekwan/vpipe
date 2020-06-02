cmake_minimum_required(VERSION 2.8)
project(vpipe)


set(OpenCV_DIR "/home/deokhwan/Tools/OpenCV/v3.4.9/share/OpenCV/")
set(CMAKE_CXX_FLAGS "-std=c++14 -g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_definitions("-DDEBUG_OPENCV")

find_package(OpenCV REQUIRED)

include_directories(
    "inc"
    "inc/feature"
    "inc/misc")

FILE(GLOB_RECURSE sourcefiles "src/*.cc")

add_executable(vpipe ${sourcefiles} main.cc)
target_link_libraries(vpipe ${OpenCV_LIBS} "-lpthread -lGL -lGLU -lglut")
#add_executable(vpipe_test ${sourcefiles} )
#target_link_libraries(vpipe_test ${OpenCV_LIBS})
