project (action)

set(SOURCES
  "action-manager.cc"
  "executor.cc"
)

add_library(troll::action ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll:action
  absl::strings
  glog
  troll::animation
  troll::core
  troll::proto
  troll::scripting
  troll::sound
)
