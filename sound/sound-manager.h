#ifndef TROLL_SOUND_SOUND_MANAGER_H_
#define TROLL_SOUND_SOUND_MANAGER_H_

#include <memory>
#include <string>

#include "sound/sound.h"

namespace troll {

class SoundManager {
 public:
  static SoundManager& Instance() {
    static SoundManager singleton;
    return singleton;
  }

  void Init();
  void CleanUp();

  std::unique_ptr<Music> LoadMusic(const std::string& resource) const;
  std::unique_ptr<Sound> LoadSound(const std::string& resource) const;

  void PlayMusic(const std::string& track_id);
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
  SoundManager() = default;
  ~SoundManager() = default;
};

}  // namespace troll

#endif  // TROLL_SOUND_SOUND_MANAGER_H_
