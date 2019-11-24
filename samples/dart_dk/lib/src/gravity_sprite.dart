import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/core/sprite.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';

class GavitySprite extends Sprite {
  int _intensity = 0;

  GavitySprite(String spriteId, [String id]) : super(spriteId, id) {
    onOverlap(
        spriteId: 'platform',
        eventHandler: (event) {
          if (_intensity == 0) return;

          final sprite = Sprite('platform', event.sceneNodeId[0]);
          if (position[1] > sprite.position[1]) return;

          final overlap = getOverlap(event.sceneNodeId[0]);
          move([0, -overlap.height]);
        });
  }

  void set gravity(int intensity) {
    _intensity = intensity;

    stopAnimation(scriptId: '${id}_gravity');

    if (_intensity != 0) {
      playAnimation(
          script: AnimationScript()
            ..id = '${id}_gravity'
            ..animation.addAll([
              Animation()
                ..translation = (VectorAnimation()
                  ..vec = makeVector([0, _intensity])
                  ..delay = 50),
            ]));
    }
  }
}
