project(pytroll)

set(SOURCES
  "python-engine.cc"
)

add_library(troll_python ${SOURCES})

target_link_libraries(troll_python
  ${PYTHON_LIBRARY}
  absl::strings
  glog::glog
  pybind11
  troll_action
  troll_core
  troll_input
  troll_proto
)
