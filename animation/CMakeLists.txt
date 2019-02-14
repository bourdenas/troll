project(animation)

set(SOURCES
  "animator-manager.cc"
  "animator.cc"
  "performer.cc"
  "script-animator.cc"
)

add_library(troll_animation ${SOURCES})

target_link_libraries(troll_animation
  absl::strings
  glog::glog
  range-v3
  troll_core
  troll_proto
  troll_sound
)
