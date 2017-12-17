#ifndef TROLL_SDL_INPUT_BACKEND_H_
#define TROLL_SDL_INPUT_BACKEND_H_

#include <SDL2/SDL.h>

#include "input/input-event.h"

namespace troll {

class InputBackend {
 public:
  static InputBackend& Instance() {
    static InputBackend singleton;
    return singleton;
  }

  void Init();

  InputEvent PollEvent() const;

  InputBackend(const InputBackend&) = delete;
  InputBackend& operator=(const InputBackend&) = delete;

 private:
  InputBackend() = default;
  ~InputBackend() = default;
};

}  // namespace troll

#endif  // TROLL_SDL_INPUT_BACKEND_H_
