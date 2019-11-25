import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/core/sprite.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';

import 'package:dart_dk/sprites/environment.dart';

class GavitySprite extends Sprite {
  int _intensity = 0;

  GavitySprite(String spriteId, [String id]) : super(spriteId, id) {
    onOverlap(
        spriteId: Platform.id,
        handler: (eventId, sprites) {
          if (_intensity == 0) return;

          final sprite = sprites.first;
          if (position[1] > sprite.position[1]) return;

          final overlap = getOverlap(sprite.nodeId);
          move([0, -overlap.height]);
        });
  }

  void set gravity(int intensity) {
    _intensity = intensity;

    stopAnimation(scriptId: '${nodeId}_gravity');

    if (_intensity != 0) {
      playAnimation(
          script: AnimationScript()
            ..id = '${nodeId}_gravity'
            ..animation.addAll([
              Animation()
                ..translation = (VectorAnimation()
                  ..vec = makeVector([0, _intensity])
                  ..delay = 50),
            ]));
    }
  }
}
