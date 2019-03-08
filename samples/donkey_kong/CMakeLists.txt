project (donkey_kong)

set(SOURCES
  "main.cc"
)

add_executable(donkey_kong ${SOURCES})

# Depend on a libraries defined in the top-level file.
target_link_libraries(donkey_kong
  troll_core
  troll_python
)
