import 'package:dart_troll/src/core/scene.dart';
import 'package:dart_troll/src/dk/sprites.dart';
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

  void setup() {
    final platformSpecs = [
      _PlatformSpec(3, [262, 97, -3]),
      _PlatformSpec(13, [80, 155, -3]),
      _PlatformSpec(13, [112, 210, -3]),
      _PlatformSpec(13, [80, 275, -3]),
      _PlatformSpec(13, [112, 335, -3]),
      _PlatformSpec(13, [80, 395, -3]),
      _PlatformSpec(14, [80, 455, -3]),
    ];

    final platforms = platformSpecs.map<Platform>((spec) {
      final platform = Platform(spec.size);
      platform.create(spec.position);
      return platform;
    }).toList();

    final ladderPositions = [
      [210, 72, -1],
      [242, 72, -1],
      [210, 112, -1],
      [242, 112, -1],
      [338, 112, -1],
      [305, 170, -1],
      [338, 170, -1],
      [305, 210, -1],
      [338, 210, -1],
      [305, 250, -1],
      [338, 250, -1],
      [305, 290, -1],
      [338, 290, -1],
      [305, 330, -1],
      [338, 330, -1],
      [305, 370, -1],
      [338, 370, -1],
      [305, 410, -1],
      [338, 410, -1],
    ];

    ladderPositions.forEach((position) {
      Ladder().create(position, 0);
    });
  }
}

class _PlatformSpec {
  int size;
  List<int> position;
  _PlatformSpec(this.size, this.position);
}
