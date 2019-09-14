import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/core/sprite.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';
import 'package:dart_troll/src/proto/event.pb.dart';

enum _MarioState {
  none,
  walk_left,
  walk_right,
  climb_up,
  climb_down,
  jump,
}

class Mario extends GavitySprite {
  _MarioState _state = _MarioState.none;
  final _animationMap = <_MarioState, String>{
    _MarioState.walk_left: 'mario_move_left',
    _MarioState.walk_right: 'mario_move_right',
    _MarioState.climb_up: 'mario_climb_up',
    _MarioState.climb_down: 'mario_climb_down',
  };

  Mario(List<int> position, {int frameIndex = 0}) : super('mario') {
    create(position, frameIndex: frameIndex);
    gravity = 8;

    onOverlap(
        spriteId: 'ladder',
        eventHandler: (event) {
          final overlap = this.getOverlap(event.sceneNodeId[0]);
          // print('overlap with ladder=${overlap.width}');
        });
  }

  void walkLeft() {
    if (_state != _MarioState.none) return;
    _state = _MarioState.walk_left;
    playAnimation(scriptId: _animationMap[_state]);
  }

  void walkRight() {
    if (_state != _MarioState.none) return;
    _state = _MarioState.walk_right;
    playAnimation(scriptId: _animationMap[_state]);
  }

  void climbUp() {
    if (_state != _MarioState.none) return;
    _state = _MarioState.climb_up;
    playAnimation(scriptId: _animationMap[_state]);
  }

  void climbDown() {
    if (_state != _MarioState.none) return;
    _state = _MarioState.climb_down;
    playAnimation(scriptId: _animationMap[_state]);
  }

  void halt() {
    final scriptId = _animationMap[_state];
    if (scriptId == null) return;

    stopAnimation(scriptId: scriptId);
    _state = _MarioState.none;
  }

  void jump() {
    playAnimation(scriptId: 'mario_jump', onDone: (e) => this.gravity = 8);
    gravity = 0;
  }
}

class DonkeyKong extends Sprite {
  DonkeyKong(List<int> position, {int frameIndex = 0}) : super('dk') {
    create(position, frameIndex: frameIndex);
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
            ..vec = makeVector([4, 0])
            ..delay = 30)
          ..frameRange = (FrameRangeAnimation()
            ..startFrame = 1
            ..endFrame = 5
            ..delay = 200),
      ]);
    playAnimation(script: script);
  }
}

class GavitySprite extends Sprite {
  int _intensity = 0;

  GavitySprite(String spriteId, [String id]) : super(spriteId, id) {
    onOverlap(
        spriteId: 'platform',
        eventHandler: (event) {
          if (_intensity == 0) return;
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
