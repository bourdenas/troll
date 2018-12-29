project (input)

set(SOURCES
  "input-manager.cc"
)

add_library(troll::input ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll::input
  range-v3
  troll::action
  troll::core
  troll::proto
)
