#include <glog/logging.h>

#include "core/troll-core.h"

int main(int argc, char *argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Troll climbing the bridge...";

  troll::Core::Instance().Init();
  troll::Core::Instance().Run();
  troll::Core::Instance().CleanUp();

  return 0;
}
