import 'package:dart_dk/level1.dart';
import 'package:dart_dk/src/sprites/donkey_kong.dart';
import 'package:dart_troll/src/core/audio.dart';
import 'package:dart_troll/src/core/scene.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';
import 'package:dart_troll/src/proto/primitives.pb.dart';
import 'package:dart_troll/src/proto/scene.pb.dart' as proto;

class HeightScene extends Scene {
  @override
  proto.Scene sceneDefinition() {
    return proto.Scene()
      ..id = 'height'
      ..viewport = (Box()
        ..width = 640
        ..height = 480);
  }

  @override
  void setup() {
    DonkeyKong([300, 282], frameIndex: 10)
      ..playAnimation(
          script: (AnimationScript()
            ..id = 'height'
            ..animation.addAll([
              Animation()..timer = (TimerAnimation()..delay = 3500),
            ])),
          onDone: (Event) => transition(Level1Scene()));

    playMusic('height');
  }
}
