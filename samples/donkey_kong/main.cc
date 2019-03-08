#define SDL_MAIN_HANDLED

#include "core/resource-manager.h"
#include "core/scripting-engine.h"
#include "core/troll-core.h"

int main(int argc, char *argv[]) {
  troll::TrollCore core;
  core.Init(argv[0], "../samples/donkey_kong/data/");
  core.scripting_engine()->CreateScene("dk.intro.IntroScene");
  core.Run();

  return 0;
}
