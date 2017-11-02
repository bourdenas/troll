#include <iostream>

#include "core/troll-core.h"
#include "proto/sprite.pb.h"

int main(int argc, char *args[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::cout << "Troll climbing the bridge...\n";

  troll::Core::Init();
  troll::Core::Run();
  troll::Core::CleanUp();

  return 0;
}
