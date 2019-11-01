import 'package:dart_troll/src/core/audio.dart';
import 'package:dart_troll/src/core/scene.dart';
import 'package:dart_troll/src/proto/primitives.pb.dart';
import 'package:dart_troll/src/proto/scene.pb.dart' as proto;

import 'package:dart_dk/sprites/donkey_kong.dart';
import 'package:dart_dk/sprites/environment.dart';
import 'package:dart_dk/sprites/mario.dart';
import 'package:dart_dk/sprites/princess.dart';

class Level1Scene extends Scene {
  @override
  proto.Scene sceneDefinition() {
    return proto.Scene()
      ..id = 'level1'
      ..viewport = (Box()
        ..width = 640
        ..height = 480);
  }

  @override
  void setup() {
    Scaffold()..buildStage();
    Princess([270, 50, -1], frameIndex: 1);
    DonkeyKong([126, 90]).throwBarrels();
    Mario([140, 428], frameIndex: 15, inputHandler: inputHandler);

    playMusic('main_loop', repeat: 0);
  }
}
