import 'package:dart_dk/height.dart';
import 'package:dart_dk/src/sprites/donkey_kong.dart';
import 'package:dart_dk/src/sprites/environment.dart';
import 'package:dart_troll/src/core/audio.dart';
import 'package:dart_troll/src/core/scene.dart';
import 'package:dart_troll/src/proto/primitives.pb.dart';
import 'package:dart_troll/src/proto/scene.pb.dart' as proto;

class IntroScene extends Scene {
  @override
  proto.Scene sceneDefinition() {
    return proto.Scene()
      ..id = 'intro'
      ..viewport = (Box()
        ..width = 640
        ..height = 480);
  }

  @override
  void setup() {
    final scaffold = Scaffold();
    final rollingLadder = RollingLadder();

    DonkeyKong([300, 382, 0], frameIndex: 4)
      ..introCutScene(scaffold, rollingLadder,
          onDone: (e) => transition(HeightScene()));

    registerKey('SPACE', onPressed: () => transition(HeightScene()));

    playMusic('intro');
  }
}
