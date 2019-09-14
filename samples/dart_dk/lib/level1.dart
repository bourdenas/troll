import 'package:dart_dk/src/sprites/donkey_kong.dart';
import 'package:dart_dk/src/sprites/environment.dart';
import 'package:dart_dk/src/sprites/mario.dart';
import 'package:dart_dk/src/sprites/princess.dart';
import 'package:dart_troll/src/core/audio.dart';
import 'package:dart_troll/src/core/scene.dart';
import 'package:dart_troll/src/proto/primitives.pb.dart';
import 'package:dart_troll/src/proto/scene.pb.dart' as proto;

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
    final scaffold = Scaffold();
    scaffold.buildStage();

    Princess([270, 50, -1], frameIndex: 1);
    DonkeyKong([126, 90]).throwBarrels();

    final mario = Mario([140, 428], frameIndex: 15);
    registerKey('LEFT',
        onPressed: () => mario.walkLeft(), onReleased: () => mario.halt());
    registerKey('RIGHT',
        onPressed: () => mario.walkRight(), onReleased: () => mario.halt());
    registerKey('UP',
        onPressed: () => mario.climbUp(), onReleased: () => mario.halt());
    registerKey('DOWN',
        onPressed: () => mario.climbDown(), onReleased: () => mario.halt());
    registerKey('SPACE', onPressed: () => mario.jump());

    playMusic('main_loop', repeat: 0);
  }
}
