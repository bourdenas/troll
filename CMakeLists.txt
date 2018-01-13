# cmake .. -DCMAKE_TOOLCHAIN_FILE=../../vcpkg/scripts/buildsystems/vcpkg.cmake

cmake_minimum_required(VERSION 3.0)
project(troll)

include_directories(".")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Boost 1.66) 

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

find_package(glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIR})

file(GLOB SOURCES
  "animation/*.cc"
  "core/*.cc"
  "input/*.cc"
  "proto/*.cc"
  "sdl/*.cc"
)

add_executable(main ${SOURCES})
target_link_libraries(main 
  ${Boost_FILESYSTEM_LIBRARY}
  ${PROTOBUF_LIBRARY}
  ${SDL2_LIBRARIES}
)
