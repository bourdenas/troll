project (scripting)

set(SOURCES
  "script-manager.cc"
)

add_library(troll_scripting ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll_scripting
  absl::strings
  glog
  pybind11
  python2.7
  troll_action
  troll_core
  troll_input
  troll_proto
)
