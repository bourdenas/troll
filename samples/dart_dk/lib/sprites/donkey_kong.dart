import 'package:dart_troll/src/core/sprite.dart';
import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';

import 'package:dart_dk/sprites/environment.dart';
import 'package:dart_dk/sprites/princess.dart';

class DonkeyKong extends Sprite {
  static final id = 'dk';

  DonkeyKong(List<int> position, {int frameIndex = 0}) : super(id) {
    create(position, frameIndex: frameIndex);
  }

  void introCutScene(Scaffold scaffold, RollingLadder ladder,
      {EventHandler onDone}) {
    final introScript = AnimationScript()
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
    playAnimation(script: introScript, onDone: onDone);

    onScriptDone('dk_landing', (e) {
      Princess([270, 50, -1], frameIndex: 1);
      scaffold.collapsePlatform(1);
      ladder.destroyTopSegment();
    });

    int count = 2;
    onScriptRewind('dk_jump', (e) {
      scaffold.collapsePlatform(count++);
    });
    onScriptDone('dk_jump', (e) => scaffold.collapsePlatform(count++));
  }

  void throwBarrels() {
    playAnimation(scriptId: 'dk_barrel_throw', onPartDone: {
      'place_barrel': (Event) {
        Barrel([215, 134], frameIndex: 1)
          ..Roll()
          ..gravity = 16;
      },
    });
  }
}
