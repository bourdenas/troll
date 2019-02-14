project(sound)

set(SOURCES
  "audio-mixer.cc"
  "sound-loader.cc"
)

add_library(troll_sound ${SOURCES})

target_link_libraries(troll_sound
  absl::strings
  glog::glog
  SDL2_mixer
  troll_core
  troll_proto
)
