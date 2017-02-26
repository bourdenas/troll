#include <cstdio>

#include "../build/sprite.pb.h"
#include "troll-core.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  printf("hello there\n");

  troll::Core::Init();
  troll::Core::Run();
  troll::Core::CleanUp();

  return 0;
}
