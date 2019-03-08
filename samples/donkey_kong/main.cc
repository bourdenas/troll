#define SDL_MAIN_HANDLED

#include <memory>

#include "core/resource-manager.h"
#include "core/troll-core.h"
#include "pytroll/python-engine.h"

constexpr char kResourceBasePath[] = "../samples/donkey_kong/data/";

int main(int argc, char *argv[]) {
  troll::TrollCore core;

  core.Init(argv[0], kResourceBasePath,
            new troll::PythonEngine(kResourceBasePath, &core));
  core.scripting_engine()->CreateScene("dk.intro.IntroScene");
  core.Run();

  return 0;
}
