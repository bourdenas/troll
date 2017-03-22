project (troll)
cmake_minimum_required(VERSION 2.8)
include(../vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(Boost COMPONENTS system filesystem REQUIRED)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
file(GLOB ProtoFiles "proto/*.proto")
protobuf_generate_cpp(ProtoSources ProtoHeaders ${ProtoFiles})

include_directories(../vcpkg/packages/range-v3_x86-windows/include)

include_directories("release")
file(GLOB SOURCES "core/*.cc")

add_executable(troll ${SOURCES} ${ProtoSources} ${ProtoHeaders})
target_link_libraries(troll
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${PROTOBUF_LIBRARY}
)
