#include "sound/sound-manager.h"

#include <SDL2/SDL_mixer.h>
#include <glog/logging.h>

namespace troll {

namespace {
constexpr char* kResourcePath = "../data/resources/sounds/";
}  // namespace

void SoundManager::Init() {
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
    LOG(ERROR) << Mix_GetError();
  }
}

void SoundManager::CleanUp() { Mix_Quit(); }

std::unique_ptr<Music> SoundManager::LoadMusic(
    const std::string& resource) const {
  auto* music = Mix_LoadMUS((kResourcePath + resource).c_str());
  if (music == NULL) {
    LOG(ERROR) << Mix_GetError();
  }
  return std::make_unique<Music>(music);
}

std::unique_ptr<Sound> SoundManager::LoadSound(
    const std::string& resource) const {
  auto* sound = Mix_LoadWAV((kResourcePath + resource).c_str());
  if (sound == NULL) {
    LOG(ERROR) << Mix_GetError();
  }
  return std::make_unique<Sound>(sound);
}

}  // namespace troll
