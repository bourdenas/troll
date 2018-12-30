project (sdl)

set(SOURCES
  "input-backend.cc"
  "renderer.cc"
  "texture.cc"
)

add_library(troll_sdl ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll_sdl
  SDL2
  SDL2_image
  SDL2_ttf
  SDL2_mixer
  absl::strings
  glog
  troll_input
  troll_proto
)
