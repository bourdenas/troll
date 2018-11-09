#include "sdl/input-backend.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "input/input-manager.h"
#include "proto/key-binding.pb.h"

namespace troll {

void InputBackend::Init() {
  SDL_SetRelativeMouseMode(SDL_TRUE);

  key_mapping_ = {
      {SDLK_DOLLAR, "DOLLAR"},
      {SDLK_AMPERSAND, "AMPERSAND"},
      {SDLK_QUOTE, "QUOTE"},
      {SDLK_LEFTPAREN, "LEFTPAREN"},
      {SDLK_RIGHTPAREN, "RIGHTPAREN"},
      {SDLK_ASTERISK, "ASTERISK"},
      {SDLK_PLUS, "PLUS"},
      {SDLK_COMMA, "COMMA"},
      {SDLK_MINUS, "MINUS"},
      {SDLK_PERIOD, "PERIOD"},
      {SDLK_SLASH, "SLASH"},

      {SDLK_0, "0"},
      {SDLK_1, "1"},
      {SDLK_2, "2"},
      {SDLK_3, "3"},
      {SDLK_4, "4"},
      {SDLK_5, "5"},
      {SDLK_6, "6"},
      {SDLK_7, "7"},
      {SDLK_8, "8"},
      {SDLK_9, "9"},

      {SDLK_COLON, "COLON"},
      {SDLK_SEMICOLON, "SEMICOLON"},
      {SDLK_LESS, "LESS"},
      {SDLK_EQUALS, "EQUALS"},
      {SDLK_GREATER, "GREATER"},
      {SDLK_QUESTION, "QUESTION"},
      {SDLK_AT, "AT"},
      {SDLK_LEFTBRACKET, "LEFTBRACKET"},
      {SDLK_BACKSLASH, "BACKSLASH"},
      {SDLK_RIGHTBRACKET, "RIGHTBRACKET"},
      {SDLK_CARET, "CARET"},
      {SDLK_UNDERSCORE, "UNDERSCORE"},
      {SDLK_BACKQUOTE, "BACKQUOTE"},
      {SDLK_DELETE, "DELETE"},

      {SDLK_a, "a"},
      {SDLK_b, "b"},
      {SDLK_c, "c"},
      {SDLK_d, "d"},
      {SDLK_e, "e"},
      {SDLK_f, "f"},
      {SDLK_g, "g"},
      {SDLK_h, "h"},
      {SDLK_i, "i"},
      {SDLK_j, "j"},
      {SDLK_k, "k"},
      {SDLK_l, "l"},
      {SDLK_m, "m"},
      {SDLK_n, "n"},
      {SDLK_o, "o"},
      {SDLK_p, "p"},
      {SDLK_q, "q"},
      {SDLK_r, "r"},
      {SDLK_s, "s"},
      {SDLK_t, "t"},
      {SDLK_u, "u"},
      {SDLK_v, "v"},
      {SDLK_w, "w"},
      {SDLK_x, "x"},
      {SDLK_y, "y"},
      {SDLK_z, "z"},

      {SDLK_KP_0, "KP_0"},
      {SDLK_KP_1, "KP_1"},
      {SDLK_KP_2, "KP_2"},
      {SDLK_KP_3, "KP_3"},
      {SDLK_KP_4, "KP_4"},
      {SDLK_KP_5, "KP_5"},
      {SDLK_KP_6, "KP_6"},
      {SDLK_KP_7, "KP_7"},
      {SDLK_KP_8, "KP_8"},
      {SDLK_KP_9, "KP_9"},
      {SDLK_KP_PERIOD, "KP_PERIOD"},
      {SDLK_KP_DIVIDE, "KP_DIVIDE"},
      {SDLK_KP_MULTIPLY, "KP_MULTIPLY"},
      {SDLK_KP_MINUS, "KP_MINUS"},
      {SDLK_KP_PLUS, "KP_PLUS"},
      {SDLK_KP_ENTER, "KP_ENTER"},
      {SDLK_KP_EQUALS, "KP_EQUALS"},

      {SDLK_UP, "UP"},
      {SDLK_DOWN, "DOWN"},
      {SDLK_RIGHT, "RIGHT"},
      {SDLK_LEFT, "LEFT"},
      {SDLK_SPACE, "SPACE"},
      {SDLK_RETURN, "RETURN"},
      {SDLK_INSERT, "INSERT"},
      {SDLK_HOME, "HOME"},
      {SDLK_END, "END"},
      {SDLK_PAGEUP, "PAGEUP"},
      {SDLK_PAGEDOWN, "PAGEDOWN"},

      {SDLK_F1, "F1"},
      {SDLK_F2, "F2"},
      {SDLK_F3, "F3"},
      {SDLK_F4, "F4"},
      {SDLK_F5, "F5"},
      {SDLK_F6, "F6"},
      {SDLK_F7, "F7"},
      {SDLK_F8, "F8"},
      {SDLK_F9, "F9"},
      {SDLK_F10, "F10"},
      {SDLK_F11, "F11"},
      {SDLK_F12, "F12"},

      {SDLK_NUMLOCKCLEAR, "NUMLOCK"},
      {SDLK_CAPSLOCK, "CAPSLOCK"},
      {SDLK_SCROLLLOCK, "SCROLLLOCK"},
      {SDLK_LSHIFT, "LSHIFT"},
      {SDLK_RSHIFT, "RSHIFT"},
      {SDLK_LCTRL, "LCTRL"},
      {SDLK_RCTRL, "RCTRL"},
      {SDLK_LALT, "LALT"},
      {SDLK_RALT, "RALT"},
      {SDLK_LGUI, "LMETA"},
      {SDLK_RGUI, "RMETA"},

      {SDL_BUTTON_LEFT, "LeftClick"},
      {SDL_BUTTON_RIGHT, "RightClick"},
      {SDL_BUTTON_MIDDLE, "MiddleClick"},
  };
}

InputEvent InputBackend::PollEvent() {
  InputEvent event;

  SDL_Event sdl_event;
  if (SDL_PollEvent(&sdl_event) == 0) {
    event.mutable_no_event();
    return event;
  }

  switch (sdl_event.type) {
    case SDL_QUIT: {
      event.mutable_quit_event();
      return event;
    }

    case SDL_MOUSEMOTION: {
      auto* mouse_event = event.mutable_mouse_event();
      auto* pos = mouse_event->mutable_absolute_position();
      pos->set_x(sdl_event.motion.x);
      pos->set_y(sdl_event.motion.y);
      pos = mouse_event->mutable_relative_position();
      pos->set_x(sdl_event.motion.xrel);
      pos->set_y(sdl_event.motion.yrel);
      return event;
    }

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      auto* mouse_event = event.mutable_mouse_event();
      mouse_event->set_button(key_mapping_[sdl_event.button.button]);
      mouse_event->set_key_state(sdl_event.button.type == SDL_MOUSEBUTTONDOWN
                                     ? Trigger::PRESSED
                                     : Trigger::RELEASED);
      auto* pos = mouse_event->mutable_absolute_position();
      pos->set_x(sdl_event.button.x);
      pos->set_y(sdl_event.button.y);
      return event;
    }

    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      auto* key_event = event.mutable_key_event();
      key_event->set_key(key_mapping_[sdl_event.key.keysym.sym]);
      key_event->set_key_state(
          sdl_event.key.state == SDL_PRESSED && !sdl_event.key.repeat
              ? Trigger::PRESSED
              : (sdl_event.key.state == SDL_RELEASED ? Trigger::RELEASED
                                                     : Trigger::NONE));
      return event;
    }

    default: {
      event.mutable_no_event();
      return event;
    }
  }
}

}  // namespace troll
