#include <iostream>

#include "proto/sprite.pb.h"
#include "troll-core.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::cout << "Troll climbing the bridge...\n";

  troll::Core::Init();
  troll::Core::Run();
  troll::Core::CleanUp();

  return 0;
}
