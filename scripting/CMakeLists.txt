project (scripting)

set(SOURCES
  "script-manager.cc"
)

add_library(troll::scripting ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll::scripting
  absl::strings
  glog
  pybind11
  troll::action
  troll::core
  troll::input
  troll::proto
)
