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
  ${PROTOBUF_LIBRARY}
  absl::strings
  glog
  range-v3
  stdc++fs
  troll_action
  troll_animation
  troll_input
  troll_proto
  troll_scripting
  troll_sdl
  troll_sound
)