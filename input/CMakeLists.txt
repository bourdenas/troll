project (input)

set(SOURCES
  "input-manager.cc"
)

add_library(troll_input ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll_input
  range-v3
  troll_action
  troll_core
  troll_proto
)
