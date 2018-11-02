#include "sound/audio-mixer.h"

#include <SDL2/SDL_mixer.h>
#include <glog/logging.h>

#include "core/resource-manager.h"

namespace troll {

void AudioMixer::PlayMusic(const std::string& track_id, int repeat = 1) {
  const auto& music = ResourceManager::Instance().GetMusic(track_id);
  // For SDL-mixer repeat 0 is play 0 times and -1 is endless loop.
  Mix_PlayMusic(music.music(), repeat != 0 ? repeat : -1);
}

void AudioMixer::StopMusic() { Mix_HaltMusic(); }

void AudioMixer::PauseMusic() { Mix_PauseMusic(); }

void AudioMixer::ResumeMusic() {
  if (Mix_PausedMusic() == 1) {
    Mix_ResumeMusic();
  }
}

bool AudioMixer::IsMusicPlaying() { return Mix_PlayingMusic() == 1; }

}  // namespace troll
