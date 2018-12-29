project (sound)

set(SOURCES
  "audio-mixer.cc"
  "sound-loader.cc"
)

add_library(troll_sound ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll_sound
  ${SDL2_LIBRARIES}
  absl::strings
  glog
  troll_core
  troll_proto
)
