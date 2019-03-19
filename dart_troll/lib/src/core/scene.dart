import 'dart:typed_data';

import 'package:dart_troll/dart_troll.dart' as troll;
import 'package:dart_troll/src/proto/action.pb.dart';
import 'package:dart_troll/src/proto/input-event.pb.dart';
import 'package:dart_troll/src/proto/scene.pb.dart' as proto;

/// Abstract class for all game scenes that handles basic setup and transition
/// of scenes.
abstract class Scene {
  /// Called before scene initialisation. Override this function to create
  /// Scene definition.
  proto.Scene sceneDefinition();

  /// Called during scene initialisation. Override to setup the scene with
  /// sprites or any other operations needed during startup.
  void setup();

  /// Called just before moving away from the scene. Override to perform any
  /// clean-up operation.
  void halt() {}
  void handleInput(InputEvent event);

  /// Starts the initialisation of the scene.
  void start() {
    final action = Action()
      ..changeScene = (ChangeSceneAction()..scene = sceneDefinition());
    troll.execute(action.writeToBuffer());

    setup();

    _input_handler_id =
        troll.registerInputHandler((Uint8List inputEventBuffer) {
      final event = InputEvent()..mergeFromBuffer(inputEventBuffer);
      handleInput(event);
    });
  }

  /// Transition to a new scene from this one.
  void transition(Scene newScene) {
    troll.unregisterInputHandler(_input_handler_id);
    halt();

    newScene.start();
  }

  int _input_handler_id;
}
