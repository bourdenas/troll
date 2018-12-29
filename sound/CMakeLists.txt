project (sound)

set(SOURCES
  "audio-mixer.cc"
  "sound-loader.cc"
)

add_library(troll::sound ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll::sound
  ${SDL2_LIBRARIES}
  absl::strings
  glog
  troll::core
  troll::proto
)
