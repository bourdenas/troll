project (proto)

set(PROTOS
  "action.proto"
  "animation.proto"
  "audio.proto"
  "event.proto"
  "input-event.proto"
  "key-binding.proto"
  "primitives.proto"
  "scene-node.proto"
  "scene.proto"
  "sprite.proto"
)

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${PROTOS})
protobuf_generate_python(PROTO_PY ${PROTOS})

add_library(troll_proto ${PROTO_HEADER} ${PROTO_SRC})

target_link_libraries(troll_proto
  ${PROTOBUF_LIBRARY}
)
