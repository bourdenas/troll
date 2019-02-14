# troll

## Build Instructions

`cmake .. -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release`

`cmake --build .`

For using vcpkg:
`-DCMAKE_TOOLCHAIN_FILE="<vcpkg-root>\scripts\buildsystems\vcpkg.cmake"`

For using ninja build system:
`-GNinja`


## Generate Protobufs manually

### C++
`protoc --cpp_out=. @protos`

### Python
`protoc --python_out=. @protos`
