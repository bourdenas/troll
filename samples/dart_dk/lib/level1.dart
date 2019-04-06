import 'package:dart_dk/sprites.dart';
import 'package:dart_troll/src/core/audio.dart';
import 'package:dart_troll/src/core/scene.dart';
import 'package:dart_troll/src/proto/event.pb.dart';
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
    final platformSpecs = [
      _PlatformSpec(3, [230, 97, -3]),
      _PlatformSpec(14, [48, 155, -3]),
      _PlatformSpec(13, [112, 210, -3]),
      _PlatformSpec(13, [80, 275, -3]),
      _PlatformSpec(13, [112, 335, -3]),
      _PlatformSpec(13, [80, 395, -3]),
      _PlatformSpec(14, [80, 455, -3]),
    ];

    final platforms = platformSpecs
        .asMap()
        .map((index, spec) =>
            MapEntry(index, Platform(index, spec.size)..create(spec.position)))
        .values
        .toList();

    for (final platform in platforms.sublist(1)) {
      platform.collapse();
    }

    final brokenLadderPositions = [
      [250, 446, -5],
      [250, 416, -5],
      [220, 326, -5],
      [220, 295, -5],
      [380, 266, -5],
      [380, 230, -5],
      [274, 198, -5],
      [274, 170, -5],
    ];
    brokenLadderPositions
        .map<Ladder>((position) => Ladder()..create(position, frameIndex: 1))
        .toList();

    final ladderPositions = [
      [210, 72, -5],
      [242, 72, -5],
      [210, 112, -5],
      [242, 112, -5],
      [338, 112, -5],
      [444, 420, -5],
      [280, 358, -5],
      [148, 353, -5],
      [310, 297, -5],
      [444, 294, -5],
      [148, 235, -5],
      [226, 235, -5],
      [444, 172, -5],
    ];
    ladderPositions
        .map<Ladder>((position) => Ladder()..create(position))
        .toList();

    Princess().create([270, 50, -1], frameIndex: 1);
    Barrel().create([80, 88], frameIndex: 5);
    Barrel().create([100, 428], frameIndex: 6);

    DonkeyKong()
      ..create([126, 90])
      ..playAnimationScriptById('dk_barrel_throw', onPartDone: {
        'place_barrel': (Event) {
          Barrel()
            ..create([215, 134], frameIndex: 1)
            ..Roll();
        },
      });

    mario = Mario()..create([140, 428], frameIndex: 15);

    playMusic('main_loop', repeat: 0);

    registerKey('LEFT',
        onPressed: () => mario.playAnimationScriptById('mario_move_left'),
        onReleased: () => mario.stopAnimationScript('mario_move_left'));
    registerKey('RIGHT',
        onPressed: () => mario.playAnimationScriptById('mario_move_right'),
        onReleased: () => mario.stopAnimationScript('mario_move_right'));
    registerKey('UP',
        onPressed: () => mario.playAnimationScriptById('mario_climb_up'),
        onReleased: () => mario.stopAnimationScript('mario_climb_up'));
    registerKey('DOWN',
        onPressed: () => mario.playAnimationScriptById('mario_climb_down'),
        onReleased: () => mario.stopAnimationScript('mario_climb_down'));
    registerKey('SPACE',
        onPressed: () => mario.playAnimationScriptById('mario_jump'));
  }

  Mario mario;
}

class _PlatformSpec {
  int size;
  List<int> position;
  _PlatformSpec(this.size, this.position);
}
