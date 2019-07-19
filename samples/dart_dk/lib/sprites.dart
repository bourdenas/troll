import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/core/sprite.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';
import 'package:dart_troll/src/proto/event.pb.dart';

class Mario extends GavitySprite {
  Mario(List<int> position, {int frameIndex = 0}) : super('mario') {
    create(position, frameIndex: frameIndex);
    gravity = [0, 1];
  }
}

class DonkeyKong extends Sprite {
  DonkeyKong(List<int> position, {int frameIndex = 0}) : super('dk') {
    create(position, frameIndex: frameIndex);
  }
}

class Princess extends Sprite {
  Princess(List<int> position, {int frameIndex = 0}) : super('princess') {
    create(position, frameIndex: frameIndex);
  }
}

class PlatformPiece extends Sprite {
  PlatformPiece(List<int> position) : super('platform') {
    create(position);
  }
}

class Platform {
  int _index;
  int _size;
  List<PlatformPiece> _pieces;

  static int _platformWidth = 32;

  Platform(this._index, this._size, List<int> position) {
    _pieces = List<PlatformPiece>.generate(_size, (_index) {
      position[0] += _platformWidth;
      return PlatformPiece(position);
    });
  }

  void collapse() {
    if (_index == 1) {
      for (int i = 0; i < 4; ++i) {
        _pieces[_pieces.length - i - 1].move([0, 4 - i]);
      }
    } else if (_index == 6) {
      for (int i = 0; i < 7; ++i) {
        _pieces[_pieces.length - i - 1].move([0, i]);
        _pieces[i].move([0, 7]);
      }
    } else if (_index % 2 == 0) {
      _pieces.asMap().forEach((i, piece) {
        piece.move([0, _pieces.length - i - 1]);
      });
    } else if (_index % 2 == 1) {
      _pieces.asMap().forEach((i, piece) {
        piece.move([0, i]);
      });
    }
  }
}

class Ladder extends Sprite {
  Ladder(List<int> position, {int frameIndex = 0}) : super('ladder') {
    create(position, frameIndex: frameIndex);
  }
}

class Barrel extends GavitySprite {
  Barrel(List<int> position, {int frameIndex = 0}) : super('barrel') {
    create(position, frameIndex: frameIndex);
  }

  void Roll() {
    final script = AnimationScript()
      ..id = '${id}_barrel_roll'
      ..animation.addAll([
        Animation()
          ..translation = (VectorAnimation()
            ..vec = makeVector([2, 0])
            ..delay = 30)
          ..frameRange = (FrameRangeAnimation()
            ..startFrame = 1
            ..endFrame = 5
            ..delay = 200),
      ]);
    playAnimationScript(script);
  }
}

class GavitySprite extends Sprite {
  GavitySprite(String spriteId, [String id]) : super(spriteId, id);

  void set gravity(List<int> force) {
    playAnimationScript(AnimationScript()
      ..id = '${id}_gravity'
      ..animation.addAll([
        Animation()
          ..translation = (VectorAnimation()
            ..vec = makeVector(force)
            ..delay = 100),
      ]));

    onCollision(
        spriteId: 'platform',
        eventHandler: (Event e) {
          final overlap = getOverlap(e.sceneNodeId[0]);
          print(overlap);
          move([0, -overlap.height]);
        });
  }
}
