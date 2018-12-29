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

add_library(troll::core ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll::core
  ${Boost_FILESYSTEM_LIBRARY}
  ${PROTOBUF_LIBRARY}
  absl::strings
  glog
  range-v3
  troll::action
  troll::animation
  troll::proto
  troll::input
  troll::scripting
  troll::sdl
  troll::sound
)
