#ifndef TROLL_SOUND_SOUND_LOADER_H_
#define TROLL_SOUND_SOUND_LOADER_H_

#include <memory>
#include <string>

#include "sound/sound.h"

namespace troll {

class SoundLoader {
 public:
  static SoundLoader& Instance() {
    static SoundLoader singleton;
    return singleton;
  }

  void Init();
  void CleanUp();

  std::unique_ptr<Music> LoadMusic(const std::string& resource) const;
  std::unique_ptr<Sound> LoadSound(const std::string& resource) const;

 private:
  SoundLoader() = default;
  ~SoundLoader() = default;
};

}  // namespace troll

#endif  // TROLL_SOUND_SOUND_LOADER_H_
