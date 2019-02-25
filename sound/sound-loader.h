#ifndef TROLL_SOUND_SOUND_LOADER_H_
#define TROLL_SOUND_SOUND_LOADER_H_

#include <memory>
#include <string>

#include "sound/sound.h"

namespace troll {

class SoundLoader {
 public:
  SoundLoader() = default;
  ~SoundLoader();

  void Init();

  std::unique_ptr<Music> LoadMusic(const std::string& resource) const;
  std::unique_ptr<Sound> LoadSound(const std::string& resource) const;

  SoundLoader(const SoundLoader&) = delete;
  SoundLoader& operator=(const SoundLoader&) = delete;
};

}  // namespace troll

#endif  // TROLL_SOUND_SOUND_LOADER_H_
