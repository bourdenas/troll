#ifndef TROLL_SDL_INPUT_BACKEND_H_
#define TROLL_SDL_INPUT_BACKEND_H_

#include <unordered_map>

#include <SDL2/SDL.h>

#include "proto/input-event.pb.h"

namespace troll {

class InputBackend {
 public:
  static InputBackend& Instance() {
    static InputBackend singleton;
    return singleton;
  }

  void Init();

  InputEvent PollEvent();

  InputBackend(const InputBackend&) = delete;
  InputBackend& operator=(const InputBackend&) = delete;

 private:
  InputBackend() = default;
  ~InputBackend() = default;

  // A mapping from a key code of a input backend into a semantic name.
  std::unordered_map<int, std::string> key_mapping_;
};

}  // namespace troll

#endif  // TROLL_SDL_INPUT_BACKEND_H_
