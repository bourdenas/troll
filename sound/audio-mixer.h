#ifndef TROLL_SOUND_AUDIO_MIXER_H_
#define TROLL_SOUND_AUDIO_MIXER_H_

#include <string>
#include <unordered_map>

namespace troll {

class AudioMixer {
 public:
  static AudioMixer& Instance() {
    static AudioMixer singleton;
    return singleton;
  }

  void Init();

  void PlayMusic(const std::string& track_id, int repeat);
  void StopMusic();
  void PauseMusic();
  void ResumeMusic();

  bool IsMusicPlaying();

  void PlaySound(const std::string& sfx_id, int repeat);
  void StopSound(const std::string& sfx_id);
  void PauseSound(const std::string& sfx_id);
  void ResumeSound(const std::string& sfx_id);

  void StopAll();
  void PauseAll();
  void ResumeAll();

 private:
  AudioMixer() = default;
  ~AudioMixer() = default;

  int LookupChannel(const std::string& sfx_id) const;

  friend void OnChannelFinished(int channel);
  void ChannelFinished(int channel);

  std::unordered_map<int, std::string> channel_mapping_;
};

}  // namespace troll

#endif  // TROLL_SOUND_AUDIO_MIXER_H_
