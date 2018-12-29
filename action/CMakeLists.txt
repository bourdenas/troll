project (action)

set(SOURCES
  "action-manager.cc"
  "executor.cc"
)

add_library(troll_action ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll_action
  absl::strings
  glog
  troll_animation
  troll_core
  troll_proto
  troll_scripting
  troll_sound
)
