project (core)

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

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll_core
  ${Boost_FILESYSTEM_LIBRARY}
  ${PROTOBUF_LIBRARY}
  absl::strings
  glog
  range-v3
  troll_action
  troll_animation
  troll_proto
  troll_input
  troll_scripting
  troll_sdl
  troll_sound
)
