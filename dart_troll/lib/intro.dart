library dk_scenes;

import 'package:dart_troll/src/core/scene.dart';
import 'package:dart_troll/src/proto/primitives.pb.dart';
import 'package:dart_troll/src/proto/scene.pb.dart' as troll_proto;

class IntroScene extends Scene {
  IntroScene() {
    scene = troll_proto.Scene()
      ..id = 'intro'
      ..viewport = (Box()
        ..width = 640
        ..height = 480);
  }
}
