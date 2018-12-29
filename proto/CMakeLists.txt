project (proto)

set(SOURCES
  "action.pb.cc"
  "animation.pb.cc"
  "audio.pb.cc"
  "event.pb.cc"
  "input-event.pb.cc"
  "key-binding.pb.cc"
  "primitives.pb.cc"
  "scene-node.pb.cc"
  "scene.pb.cc"
  "sprite.pb.cc"
)

add_library(troll::proto ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(troll:proto
  ${PROTOBUF_LIBRARY}
)
