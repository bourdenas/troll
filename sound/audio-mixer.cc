#include "sound/audio-mixer.h"

#include <SDL2/SDL_mixer.h>
#include <glog/logging.h>

#include "core/resource-manager.h"

namespace troll {

void OnMusicFinished() {}

void OnChannelFinished(int channel) {
  AudioMixer::Instance().ChannelFinished(channel);
}

void AudioMixer::Init() {
  Mix_HookMusicFinished(&OnMusicFinished);
  Mix_ChannelFinished(&OnChannelFinished);
}

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

void AudioMixer::PlaySound(const std::string& sfx_id, int repeat = 1) {
  const auto& sfx = ResourceManager::Instance().GetSound(sfx_id);

  int channel = Mix_PlayChannel(-1, sfx.sound(), repeat - 1);
  if (channel == -1) {
    LOG(ERROR) << Mix_GetError();
  }
  channel_mapping_.emplace(channel, sfx_id);
}

void AudioMixer::StopSound(const std::string& sfx_id) {
  const int channel = LookupChannel(sfx_id);
  if (channel == -1) {
    Mix_HaltChannel(channel);
  }
}

void AudioMixer::PauseSound(const std::string& sfx_id) {
  const int channel = LookupChannel(sfx_id);
  if (channel == -1) {
    Mix_Pause(channel);
  }
}

void AudioMixer::ResumeSound(const std::string& sfx_id) {
  const int channel = LookupChannel(sfx_id);
  if (channel == -1) {
    Mix_Resume(channel);
  }
}

int AudioMixer::LookupChannel(const std::string& sfx_id) const {
  // NB: This is serial lookup based on value of a map. This is not expected to
  // be a frequent operation plus the size of map should be fairly small.
  for (const auto& kv : channel_mapping_) {
    if (kv.second == sfx_id) {
      return kv.first;
    }
  }
  return -1;
}

void AudioMixer::ChannelFinished(int channel) {
  const auto it = channel_mapping_.find(channel);
  if (it != channel_mapping_.end()) {
    channel_mapping_.erase(it);
  }
}

}  // namespace troll
