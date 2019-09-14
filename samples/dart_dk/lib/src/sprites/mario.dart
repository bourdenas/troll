import 'package:dart_dk/src/sprites/gravity_sprite.dart';

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
