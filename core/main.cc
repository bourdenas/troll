#include <iostream>

#include "core/troll-core.h"

int main(int argc, char *args[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::cout << "Troll climbing the bridge...\n";

  troll::Core::Instance().Init();
  troll::Core::Instance().Run();
  troll::Core::Instance().CleanUp();

  return 0;
}
