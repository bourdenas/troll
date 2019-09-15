import 'dart:typed_data';

import 'package:dart_troll/dart_troll.dart' as troll;
import 'package:dart_troll/src/core/input_handler.dart';
import 'package:dart_troll/src/proto/action.pb.dart';
import 'package:dart_troll/src/proto/input-event.pb.dart';
import 'package:dart_troll/src/proto/scene.pb.dart' as proto;

/// Abstract class for all game scenes that handles basic setup and transition
/// of scenes.
abstract class Scene {
  final inputHandler = InputHandler();
  int _input_handler_id;

  /// Called before scene initialisation. Override this function to create
  /// Scene definition.
  proto.Scene sceneDefinition();

  /// Called during scene initialisation. Override to setup the scene with
  /// sprites or any other operations needed during startup.
  void setup();

  /// Called just before moving away from the scene. Override to perform any
  /// clean-up operation.
  void halt() {}

  /// Registers key handling functions for the scene.
  void registerKey(String key, {Function onPressed, Function onReleased}) {
    inputHandler.registerKey(key, onPressed: onPressed, onReleased: onReleased);
  }

  /// Transition to a new scene from this one.
  void transition(Scene newScene) {
    troll.unregisterInputHandler(_input_handler_id);
    halt();

    newScene.start();
  }

  /// Starts the initialisation of the scene.
  void start() {
    final action = Action()
      ..changeScene = (ChangeSceneAction()..scene = sceneDefinition());
    troll.execute(action.writeToBuffer());

    _input_handler_id = troll.registerInputHandler(
        (Uint8List inputEventBuffer) => inputHandler
            .handleInput(InputEvent()..mergeFromBuffer(inputEventBuffer)));

    setup();
  }
}
