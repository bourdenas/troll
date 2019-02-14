# cmake .. -DCMAKE_TOOLCHAIN_FILE=../../vcpkg/scripts/buildsystems/vcpkg.cmake
cmake_minimum_required(VERSION 3.6)
project(troll)

set(CMAKE_CXX_STANDARD 17)

include_directories(".")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
  # Visual Studio compiler cannot compile range-v3 library, use alternative.
  include_directories("../vcpkg/packages/range-v3-vs2015_x86-windows/include")
  # vcpkg is unable to find SDL2-{image,ttf,mixer}, hard-link lib path.
  link_directories("../vcpkg/installed/x86-windows/lib")
endif()

find_package(SDL2 REQUIRED)

find_package(Boost REQUIRED) 

find_package(glog REQUIRED) 

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

find_package(PythonLibs REQUIRED)

# Use -DBUILD_TESTING=OFF to disable tests.
add_subdirectory(deps/abseil-cpp)
if(!MSVC)
  add_subdirectory(deps/googletest)
endif()
add_subdirectory(deps/pybind11)
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
