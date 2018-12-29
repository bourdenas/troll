project (animation)

set(SOURCES
  "animator-manager.cc"
  "animator.cc"
  "performer.cc"
  "script-animator.cc"
)

add_library(troll::animation ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll::animation
  absl::strings
  glog
  range-v3
  troll::core
  troll::proto
  troll::sound
)
