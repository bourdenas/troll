#ifndef TROLL_SOUND_AUDIO_MIXER_H_
#define TROLL_SOUND_AUDIO_MIXER_H_

#include <functional>
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

  void PlayMusic(const std::string& track_id, int repeat,
                 const std::function<void()>& on_done);
  void StopMusic();
  void PauseMusic();
  void ResumeMusic();

  bool IsMusicPlaying();

  void PlaySound(const std::string& sfx_id, int repeat,
                 const std::function<void()>& on_done);
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

  void ChannelFinished(int channel);

  std::function<void()> on_music_done_;

  struct SfxData {
    std::string sfx_id;
    std::function<void()> on_done;
  };
  std::unordered_map<int, SfxData> channel_mapping_;

  friend void OnMusicFinished();
  friend void OnChannelFinished(int channel);
};

}  // namespace troll

#endif  // TROLL_SOUND_AUDIO_MIXER_H_
