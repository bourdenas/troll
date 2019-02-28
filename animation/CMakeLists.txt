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

add_executable(performer_test "performer_test.cc")
target_link_libraries(performer_test PRIVATE
  troll_animation
  troll_core
  Catch2::Catch2
)
catch_discover_tests(performer_test)

add_executable(script-animator_test "script-animator_test.cc")
target_link_libraries(script-animator_test PRIVATE
  troll_animation
  troll_core
  Catch2::Catch2
)
catch_discover_tests(script-animator_test)

