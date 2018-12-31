project (sdl)

set(SOURCES
  "input-backend.cc"
  "renderer.cc"
  "texture.cc"
)

add_library(troll_sdl ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll_sdl
  absl::strings
  glog
  SDL2
  SDL2_image
  SDL2_mixer
  SDL2_ttf
  troll_input
  troll_proto
)
