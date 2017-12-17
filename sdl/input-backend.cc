#include "sdl/input-backend.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "input/input-event.h"
#include "input/input-manager.h"
#include "proto/key-binding.pb.h"

namespace troll {

void InputBackend::Init() {
  SDL_SetRelativeMouseMode(SDL_TRUE);
  auto& input = InputManager::Instance();

  input.AddKeyMapping("DOLLAR", SDLK_DOLLAR);
  input.AddKeyMapping("AMPERSAND", SDLK_AMPERSAND);
  input.AddKeyMapping("QUOTE", SDLK_QUOTE);
  input.AddKeyMapping("LEFTPAREN", SDLK_LEFTPAREN);
  input.AddKeyMapping("RIGHTPAREN", SDLK_RIGHTPAREN);
  input.AddKeyMapping("ASTERISK", SDLK_ASTERISK);
  input.AddKeyMapping("PLUS", SDLK_PLUS);
  input.AddKeyMapping("COMMA", SDLK_COMMA);
  input.AddKeyMapping("MINUS", SDLK_MINUS);
  input.AddKeyMapping("PERIOD", SDLK_PERIOD);
  input.AddKeyMapping("SLASH", SDLK_SLASH);

  input.AddKeyMapping("0", SDLK_0);
  input.AddKeyMapping("1", SDLK_1);
  input.AddKeyMapping("2", SDLK_2);
  input.AddKeyMapping("3", SDLK_3);
  input.AddKeyMapping("4", SDLK_4);
  input.AddKeyMapping("5", SDLK_5);
  input.AddKeyMapping("6", SDLK_6);
  input.AddKeyMapping("7", SDLK_7);
  input.AddKeyMapping("8", SDLK_8);
  input.AddKeyMapping("9", SDLK_9);

  input.AddKeyMapping("COLON", SDLK_COLON);
  input.AddKeyMapping("SEMICOLON", SDLK_SEMICOLON);
  input.AddKeyMapping("LESS", SDLK_LESS);
  input.AddKeyMapping("EQUALS", SDLK_EQUALS);
  input.AddKeyMapping("GREATER", SDLK_GREATER);
  input.AddKeyMapping("QUESTION", SDLK_QUESTION);
  input.AddKeyMapping("AT", SDLK_AT);
  input.AddKeyMapping("LEFTBRACKET", SDLK_LEFTBRACKET);
  input.AddKeyMapping("BACKSLASH", SDLK_BACKSLASH);
  input.AddKeyMapping("RIGHTBRACKET", SDLK_RIGHTBRACKET);
  input.AddKeyMapping("CARET", SDLK_CARET);
  input.AddKeyMapping("UNDERSCORE", SDLK_UNDERSCORE);
  input.AddKeyMapping("BACKQUOTE", SDLK_BACKQUOTE);
  input.AddKeyMapping("DELETE", SDLK_DELETE);

  input.AddKeyMapping("a", SDLK_a);
  input.AddKeyMapping("b", SDLK_b);
  input.AddKeyMapping("c", SDLK_c);
  input.AddKeyMapping("d", SDLK_d);
  input.AddKeyMapping("e", SDLK_e);
  input.AddKeyMapping("f", SDLK_f);
  input.AddKeyMapping("g", SDLK_g);
  input.AddKeyMapping("h", SDLK_h);
  input.AddKeyMapping("i", SDLK_i);
  input.AddKeyMapping("j", SDLK_j);
  input.AddKeyMapping("k", SDLK_k);
  input.AddKeyMapping("l", SDLK_l);
  input.AddKeyMapping("m", SDLK_m);
  input.AddKeyMapping("n", SDLK_n);
  input.AddKeyMapping("o", SDLK_o);
  input.AddKeyMapping("p", SDLK_p);
  input.AddKeyMapping("q", SDLK_q);
  input.AddKeyMapping("r", SDLK_r);
  input.AddKeyMapping("s", SDLK_s);
  input.AddKeyMapping("t", SDLK_t);
  input.AddKeyMapping("u", SDLK_u);
  input.AddKeyMapping("v", SDLK_v);
  input.AddKeyMapping("w", SDLK_w);
  input.AddKeyMapping("x", SDLK_x);
  input.AddKeyMapping("y", SDLK_y);
  input.AddKeyMapping("z", SDLK_z);

  input.AddKeyMapping("KP_0", SDLK_KP_0);
  input.AddKeyMapping("KP_1", SDLK_KP_1);
  input.AddKeyMapping("KP_2", SDLK_KP_2);
  input.AddKeyMapping("KP_3", SDLK_KP_3);
  input.AddKeyMapping("KP_4", SDLK_KP_4);
  input.AddKeyMapping("KP_5", SDLK_KP_5);
  input.AddKeyMapping("KP_6", SDLK_KP_6);
  input.AddKeyMapping("KP_7", SDLK_KP_7);
  input.AddKeyMapping("KP_8", SDLK_KP_8);
  input.AddKeyMapping("KP_9", SDLK_KP_9);
  input.AddKeyMapping("KP_PERIOD", SDLK_KP_PERIOD);
  input.AddKeyMapping("KP_DIVIDE", SDLK_KP_DIVIDE);
  input.AddKeyMapping("KP_MULTIPLY", SDLK_KP_MULTIPLY);
  input.AddKeyMapping("KP_MINUS", SDLK_KP_MINUS);
  input.AddKeyMapping("KP_PLUS", SDLK_KP_PLUS);
  input.AddKeyMapping("KP_ENTER", SDLK_KP_ENTER);
  input.AddKeyMapping("KP_EQUALS", SDLK_KP_EQUALS);

  input.AddKeyMapping("UP", SDLK_UP);
  input.AddKeyMapping("DOWN", SDLK_DOWN);
  input.AddKeyMapping("RIGHT", SDLK_RIGHT);
  input.AddKeyMapping("LEFT", SDLK_LEFT);
  input.AddKeyMapping("INSERT", SDLK_INSERT);
  input.AddKeyMapping("HOME", SDLK_HOME);
  input.AddKeyMapping("END", SDLK_END);
  input.AddKeyMapping("PAGEUP", SDLK_PAGEUP);
  input.AddKeyMapping("PAGEDOWN", SDLK_PAGEDOWN);

  input.AddKeyMapping("F1", SDLK_F1);
  input.AddKeyMapping("F2", SDLK_F2);
  input.AddKeyMapping("F3", SDLK_F3);
  input.AddKeyMapping("F4", SDLK_F4);
  input.AddKeyMapping("F5", SDLK_F5);
  input.AddKeyMapping("F6", SDLK_F6);
  input.AddKeyMapping("F7", SDLK_F7);
  input.AddKeyMapping("F8", SDLK_F8);
  input.AddKeyMapping("F9", SDLK_F9);
  input.AddKeyMapping("F10", SDLK_F10);
  input.AddKeyMapping("F11", SDLK_F11);
  input.AddKeyMapping("F12", SDLK_F12);

  input.AddKeyMapping("NUMLOCK", SDLK_NUMLOCKCLEAR);
  input.AddKeyMapping("CAPSLOCK", SDLK_CAPSLOCK);
  input.AddKeyMapping("SCROLLLOCK", SDLK_SCROLLLOCK);
  input.AddKeyMapping("LSHIFT", SDLK_LSHIFT);
  input.AddKeyMapping("RSHIFT", SDLK_RSHIFT);
  input.AddKeyMapping("LCTRL", SDLK_LCTRL);
  input.AddKeyMapping("RCTRL", SDLK_RCTRL);
  input.AddKeyMapping("LALT", SDLK_LALT);
  input.AddKeyMapping("RALT", SDLK_RALT);
  input.AddKeyMapping("LMETA", SDLK_LGUI);
  input.AddKeyMapping("RMETA", SDLK_RGUI);

  input.AddKeyMapping("LeftClick", SDL_BUTTON_LEFT);
  input.AddKeyMapping("RightClick", SDL_BUTTON_RIGHT);
  input.AddKeyMapping("MiddleClick", SDL_BUTTON_MIDDLE);
}

InputEvent InputBackend::PollEvent() const {
  SDL_Event event;
  if (SDL_PollEvent(&event) == 0) return {};

  switch (event.type) {
    case SDL_QUIT: {
      return InputEvent::QuitEvent{};
    }

    case SDL_MOUSEMOTION: {
      return InputEvent::MouseEvent{
          int(),
          Trigger::NONE,
          {event.motion.x, event.motion.y},
          {event.motion.xrel, event.motion.yrel},
      };
    }

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      return InputEvent::MouseEvent{
          event.button.button,
          event.button.type == SDL_MOUSEBUTTONDOWN ? Trigger::PRESSED
                                                   : Trigger::RELEASED,
          {event.button.x, event.button.y},
          {0, 0},
      };
    }

    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      return InputEvent::KeyEvent{
          event.key.keysym.sym, event.key.keysym.mod,
          event.key.state == SDL_PRESSED && !event.key.repeat
              ? Trigger::PRESSED
              : (event.key.state == SDL_RELEASED ? Trigger::RELEASED
                                                 : Trigger::NONE),
      };
    }

    default: { return {}; }
  }
}

}  // namespace troll
