project (dart_troll)

add_definitions(-DDART_SHARED_LIB)

include_directories("C:/tools/dart-sdk/include")
link_directories("C:/tools/dart-sdk/bin")

set(SOURCES
  "dart_troll.cc"
  "dart_utils.cc"
)

add_library(dart_troll SHARED ${SOURCES})

target_link_libraries(dart_troll
  absl::strings
  troll_core
  dart
)
