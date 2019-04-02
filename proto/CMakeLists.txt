project(proto)

set(PROTOS
  "action.proto"
  "animation.proto"
  "audio.proto"
  "event.proto"
  "input-event.proto"
  "key-binding.proto"
  "primitives.proto"
  "query.proto"
  "scene-node.proto"
  "scene.proto"
  "sprite.proto"
)

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${PROTOS})
protobuf_generate_python(PROTO_PY ${PROTOS})

add_library(troll_proto ${PROTO_HEADER} ${PROTO_SRC})

# Uncomment line below to generate python proto files.
# add_custom_target(troll_py_proto ALL DEPENDS ${PROTO_PY})
# 
# TODO: Protos will be generated in <build_folder>/proto/ and game engine
# expects them in <project_root>/proto/. For now generated manually by invoking
# protoc from terminal.

# Dart protos:
# protoc --plugin=protoc-gen-dart=C:\Users\bourd\AppData\Roaming\Pub\Cache\bin\protoc-gen-dart.bat --dart_out=../dart_troll/lib/src/proto @protos

target_link_libraries(troll_proto
  ${PROTOBUF_LIBRARY}
)
