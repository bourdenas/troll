#include "sound/sound-loader.h"

#include <SDL2/SDL_mixer.h>
#include <glog/logging.h>

namespace troll {

SoundLoader::~SoundLoader() { Mix_Quit(); }

void SoundLoader::Init() {
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
    LOG(ERROR) << Mix_GetError();
  }
}

std::unique_ptr<Music> SoundLoader::LoadMusic(
    const std::string& resource) const {
  auto* music = Mix_LoadMUS(resource.c_str());
  if (music == NULL) {
    LOG(ERROR) << Mix_GetError();
  }
  return std::make_unique<Music>(music);
}

std::unique_ptr<Sound> SoundLoader::LoadSound(
    const std::string& resource) const {
  auto* sound = Mix_LoadWAV(resource.c_str());
  if (sound == NULL) {
    LOG(ERROR) << Mix_GetError();
  }
  return std::make_unique<Sound>(sound);
}

}  // namespace troll
