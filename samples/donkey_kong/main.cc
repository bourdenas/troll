#define SDL_MAIN_HANDLED

#include "core/resource-manager.h"
#include "core/troll-core.h"
#include "scripting/script-manager.h"

int main(int argc, char *argv[]) {
  troll::TrollCore core;
  core.Init(argv[0], "../samples/donkey_kong/data/");
  core.script_manager()->CreateScene("dk.intro", "IntroScene");
  core.Run();

  return 0;
}
