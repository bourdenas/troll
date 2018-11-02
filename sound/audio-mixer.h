#ifndef TROLL_SOUND_AUDIO_MIXER_H_
#define TROLL_SOUND_AUDIO_MIXER_H_

#include <string>

namespace troll {

class AudioMixer {
 public:
  static AudioMixer& Instance() {
    static AudioMixer singleton;
    return singleton;
  }

  void PlayMusic(const std::string& track_id, int repeat);
  void StopMusic();
  void PauseMusic();
  void ResumeMusic();

  bool IsMusicPlaying();

  void PlaySound(const std::string& sfx_id);
  void StopSound(const std::string& sfx_id);

  void StopAll();
  void PauseAll();
  void ResumeAll();

 private:
  AudioMixer() = default;
  ~AudioMixer() = default;
};

}  // namespace troll

#endif  // TROLL_SOUND_AUDIO_MIXER_H_
