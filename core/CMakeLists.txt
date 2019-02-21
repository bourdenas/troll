project(core)

set(SOURCES
  "collision-checker.cc"
  "event-dispatcher.cc"
  "events.cc"
  "execution-context.cc"
  "geometry.cc"
  "resource-manager.cc"
  "scene-manager.cc"
  "scene-node-query.cc"
  "troll-core.cc"
)

add_library(troll_core ${SOURCES})

target_link_libraries(troll_core
  ${PROTOBUF_LIBRARY}
  absl::strings
  glog::glog
  range-v3
  # stdc++fs
  troll_action
  troll_animation
  troll_input
  troll_proto
  troll_scripting
  troll_sdl
  troll_sound
)

add_executable(geometry_test "geometry_test.cc")
target_link_libraries(geometry_test PRIVATE troll_core Catch2::Catch2)
catch_discover_tests(geometry_test)

add_executable(event-dispatcher_test "event-dispatcher_test.cc")
target_link_libraries(event-dispatcher_test PRIVATE troll_core Catch2::Catch2)
catch_discover_tests(event-dispatcher_test)
