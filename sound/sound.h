#ifndef TROLL_SOUND_SOUND_H_
#define TROLL_SOUND_SOUND_H_

#include <memory>
#include <string>

#include <SDL2/SDL_mixer.h>

namespace troll {

class Music {
 public:
  Music(Mix_Music* music) : music_(music) {}
  Music(const Music&) = delete;
  ~Music() { Mix_FreeMusic(music_); }

  Mix_Music* music() const { return music_; }

 private:
  Mix_Music* music_;
};

class Sound {
 public:
  Sound(Mix_Chunk* sound) : sound_(sound) {}
  Sound(const Sound&) = delete;
  ~Sound() { Mix_FreeChunk(sound_); }

  Mix_Chunk* sound() const { return sound_; }

 private:
  Mix_Chunk* sound_;
};

}  // namespace troll

#endif  // TROLL_SOUND_SOUND_H_
