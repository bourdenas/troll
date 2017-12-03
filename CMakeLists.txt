# cmake .. -DCMAKE_TOOLCHAIN_FILE=../../vcpkg/scripts/buildsystems/vcpkg.cmake

cmake_minimum_required(VERSION 3.0)
project(troll)

include_directories(".")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(Boost 1.65.1) 

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

file(GLOB SOURCES "core/*.cc")

add_executable(main ${SOURCES})
target_link_libraries(main 
  ${Boost_FILESYSTEM_LIBRARY}
  ${PROTOBUF_LIBRARY}
  ${SDL2_LIBRARY}
)
