import 'package:dart_troll/src/core/audio.dart';
import 'package:dart_troll/src/core/scene.dart';
import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/dk/height.dart';
import 'package:dart_troll/src/dk/sprites.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';
import 'package:dart_troll/src/proto/primitives.pb.dart';
import 'package:dart_troll/src/proto/scene.pb.dart' as proto;

class IntroScene extends Scene {
  IntroScene() {
    scene = proto.Scene()
      ..id = 'intro'
      ..viewport = (Box()
        ..width = 640
        ..height = 480);
  }

  void setup() {
    final platformSpecs = [
      _PlatformSpec(3, [230, 97, -3]),
      _PlatformSpec(13, [80, 155, -3]),
      _PlatformSpec(13, [112, 210, -3]),
      _PlatformSpec(13, [80, 275, -3]),
      _PlatformSpec(13, [112, 335, -3]),
      _PlatformSpec(13, [80, 395, -3]),
      _PlatformSpec(14, [80, 455, -3]),
    ];

    final platforms = platformSpecs
        .asMap()
        .map((index, spec) {
          final platform = Platform(index, spec.size);
          platform.create(spec.position);
          return MapEntry(index, platform);
        })
        .values
        .toList();

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

    final ladders = ladderPositions.reversed.map<Ladder>((position) {
      return Ladder()..create(position);
    }).toList();

    final ladderFadingScript = AnimationScript()
      ..animation.add(Animation()..flash = (FlashAnimation()..repeat = 1));
    for (var i = 0; i < 4; ++i) {
      ladderFadingScript.animation[0].flash.delay = 700 * (i + 1);
      ladders[i * 2].playAnimationScript(ladderFadingScript);
      ladders[i * 2 + 1].playAnimationScript(ladderFadingScript);
    }

    final script = AnimationScript()
      ..id = 'intro'
      ..animation.addAll([
        Animation()..runScript = (RunScriptAnimation()..scriptId = 'dk_climb'),
        Animation()..timer = (TimerAnimation()..delay = 1000),
        Animation()
          ..runScript = (RunScriptAnimation()..scriptId = 'dk_landing'),
        Animation()..timer = (TimerAnimation()..delay = 300),
        Animation()
          ..runScript = (RunScriptAnimation()..scriptId = 'dk_jump')
          ..goTo = (GotoAnimation()
            ..destination = makeVector([130, 90])
            ..step = 1
            ..delay = 18)
          ..termination = Animation_TerminationCondition.ALL,
        Animation()..timer = (TimerAnimation()..delay = 300),
        Animation()..runScript = (RunScriptAnimation()..scriptId = 'dk_taunt'),
        Animation()..timer = (TimerAnimation()..delay = 1000),
      ]);

    final dk = DonkeyKong()
      ..create([300, 382, 0], frameIndex: 4)
      ..playAnimationScript(script, onDone: () {
        HeightScene()
          ..transition()
          ..setup();
      });

    dk.onScriptDone('dk_landing', () {
      Princess().create([270, 50, -1], frameIndex: 1);
      platforms[1].collapse();
      for (int i = 4; i < 7; ++i) {
        ladders[i * 2].destroy();
        ladders[i * 2 + 1].destroy();
      }
    });

    dk.onScriptDone('dk_jump', () {
      platforms[2].collapse();
      dk.playAnimationScriptById('dk_jump', onDone: () {
        platforms[3].collapse();
        dk.playAnimationScriptById('dk_jump', onDone: () {
          platforms[4].collapse();
          dk.playAnimationScriptById('dk_jump', onDone: () {
            platforms[5].collapse();
            dk.playAnimationScriptById('dk_jump', onDone: () {
              platforms[6].collapse();
            });
          });
        });
      });
    });

    playMusic('intro');
  }
}

class _PlatformSpec {
  int size;
  List<int> position;
  _PlatformSpec(this.size, this.position);
}
