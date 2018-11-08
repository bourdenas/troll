#define SDL_MAIN_HANDLED

#include "core/resource-manager.h"
#include "core/troll-core.h"

int main(int argc, char *argv[]) {
  troll::Core::Instance().Init(argv[0], "../samples/donkey_kong/data/");

  troll::Core::Instance().LoadScene(
      troll::ResourceManager::Instance().LoadScene(
          "../samples/donkey_kong/data/scenes/main.scene"));
  troll::Core::Instance().Run();

  troll::Core::Instance().CleanUp();
  return 0;
}
