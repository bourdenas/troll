project (sdl)

set(SOURCES
  "input-backend.cc"
  "renderer.cc"
  "texture.cc"
)

add_library(troll::sdl ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll::sdl
  ${SDL2_LIBRARIES}
  absl::strings
  glog
  troll::input
  troll::proto
)
