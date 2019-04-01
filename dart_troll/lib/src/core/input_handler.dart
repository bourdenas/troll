import 'package:dart_troll/src/proto/input-event.pb.dart';
import 'package:dart_troll/src/proto/key-binding.pb.dart';

class InputHandler {
  InputHandler() {
    keyMapping = {};
  }

  void registerKey(String key, {Function onPressed, Function onReleased}) {
    keyMapping[key] = KeyTrigger(onPressed, onReleased);
  }

  void handleInput(InputEvent event) {
    if (!event.hasKeyEvent()) return;

    print(event);
    if (event.keyEvent.keyState == Trigger_KeyState.NONE) return;
    final trigger = keyMapping[event.keyEvent.key];
    if (trigger == null) return;

    final handler = event.keyEvent.keyState == Trigger_KeyState.PRESSED
        ? trigger.onPressed
        : trigger.onReleased;
    if (handler != null) {
      handler();
    }
  }

  Map<String, KeyTrigger> keyMapping;
}

class KeyTrigger {
  Function onPressed;
  Function onReleased;

  KeyTrigger(this.onPressed, this.onReleased);
}
