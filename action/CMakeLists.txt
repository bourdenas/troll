project(action)

set(SOURCES
  "action-manager.cc"
  "executor.cc"
)

add_library(troll_action ${SOURCES})

target_link_libraries(troll_action
  absl::strings
  glog::glog
  troll_animation
  troll_core
  troll_proto
  troll_scripting
  troll_sound
)
