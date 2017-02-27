#include <cstdio>

#include "../build/sprite.pb.h"
#include "troll-core.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  printf("Troll climbing the bridge...\n");

  troll::Core::Init();
  troll::Core::Run();
  troll::Core::CleanUp();

  return 0;
}
