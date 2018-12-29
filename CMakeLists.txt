# cmake .. -DCMAKE_TOOLCHAIN_FILE=../../vcpkg/scripts/buildsystems/vcpkg.cmake
cmake_minimum_required(VERSION 3.6)
project(troll)

set(CMAKE_CXX_STANDARD 17)

include_directories(".")
if(MSVC)
  # Visual Studio compiler cannot compile range-v3 library, use alternative.
  include_directories("../vcpkg/packages/range-v3-vs2015_x86-windows/include")
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Boost 1.66) 

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

find_package(glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIR})

add_subdirectory(deps/absl-cpp)
add_subdirectory(deps/googletest)
add_subdirectory(deps/range-v3)

add_subdirectory(action)
add_subdirectory(animation)
add_subdirectory(core)
add_subdirectory(input)
add_subdirectory(proto)
add_subdirectory(scripting)
add_subdirectory(sdl)
add_subdirectory(sound)

add_subdirectory(samples/donkey_kong)
