project(scripting)

set(SOURCES
  "script-manager.cc"
)

add_library(troll_scripting ${SOURCES})

target_link_libraries(troll_scripting
  ${PYTHON_LIBRARY}
  absl::strings
  glog::glog
  pybind11
  troll_action
  troll_core
  troll_input
  troll_proto
)
