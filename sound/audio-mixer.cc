#include "sound/audio-mixer.h"

#include <SDL2/SDL_mixer.h>
#include <glog/logging.h>

#include "core/resource-manager.h"

namespace troll {

namespace {
// NB: Awful hack for the C-style API of SDL. If two instances of AudioMixer are
// created this will blow.
AudioMixer* mixer_instance;
}  // namespace

void OnMusicFinished() {
  if (mixer_instance == nullptr) return;

  if (mixer_instance->on_music_done_) {
    mixer_instance->on_music_done_();
  }
}

void OnChannelFinished(int channel) {
  if (mixer_instance == nullptr) return;

  mixer_instance->ChannelFinished(channel);
}

AudioMixer::AudioMixer(const ResourceManager* resource_manager)
    : resource_manager_(resource_manager) {
  mixer_instance = this;
  Mix_HookMusicFinished(&OnMusicFinished);
  Mix_ChannelFinished(&OnChannelFinished);
}
AudioMixer::~AudioMixer() { mixer_instance = nullptr; }

void AudioMixer::PlayMusic(const std::string& track_id, int repeat,
                           const std::function<void()>& on_done) {
  const auto& music = resource_manager_->GetMusic(track_id);
  // For SDL-mixer repeat 0 is play 0 times and -1 is endless loop.
  Mix_PlayMusic(music.music(), repeat != 0 ? repeat : -1);
  on_music_done_ = on_done;
}

void AudioMixer::StopMusic() { Mix_HaltMusic(); }

void AudioMixer::PauseMusic() { Mix_PauseMusic(); }

void AudioMixer::ResumeMusic() {
  if (Mix_PausedMusic() == 1) {
    Mix_ResumeMusic();
  }
}

bool AudioMixer::IsMusicPlaying() { return Mix_PlayingMusic() == 1; }

void AudioMixer::PlaySound(const std::string& sfx_id, int repeat,
                           const std::function<void()>& on_done) {
  const auto& sfx = resource_manager_->GetSound(sfx_id);

  int channel = Mix_PlayChannel(-1, sfx.sound(), repeat - 1);
  if (channel == -1) {
    LOG(ERROR) << Mix_GetError();
  }
  channel_mapping_.emplace(channel, SfxData{sfx_id, on_done});
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
    if (kv.second.sfx_id == sfx_id) {
      return kv.first;
    }
  }
  return -1;
}

void AudioMixer::ChannelFinished(int channel) {
  const auto it = channel_mapping_.find(channel);
  if (it != channel_mapping_.end()) {
    if (it->second.on_done) {
      it->second.on_done();
    }
    channel_mapping_.erase(it);
  }
}

}  // namespace troll
