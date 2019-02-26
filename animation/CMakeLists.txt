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

add_executable(animator_test "animator_test.cc")
target_link_libraries(animator_test PRIVATE
  troll_animation
  troll_core
  Catch2::Catch2
)
catch_discover_tests(animator_test)
