import 'package:dart_troll/src/core/input_handler.dart';
import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';

import 'package:dart_dk/src/gravity_sprite.dart';

class Mario extends GavitySprite {
  var _state = _MarioAnimation.none;

  Mario(List<int> position, {int frameIndex = 0, InputHandler inputHandler})
      : super('mario') {
    create(position, frameIndex: frameIndex);
    gravity = 8;

    onOverlap(
        spriteId: 'ladder',
        eventHandler: (event) {
          final overlap = this.getOverlap(event.sceneNodeId[0]);
          // print('overlap with ladder=${overlap.width}');
        });

    if (inputHandler != null) {
      _setupControls(inputHandler);
    }
  }

  void _setupControls(InputHandler inputHandler) {
    inputHandler.registerKey('LEFT',
        onPressed: () => _walkLeft(),
        onReleased: () => _halt(_MarioAnimation.moveLeft));
    inputHandler.registerKey('RIGHT',
        onPressed: () => _walkRight(),
        onReleased: () => _halt(_MarioAnimation.moveRight));
    inputHandler.registerKey('UP',
        onPressed: () => _climbUp(),
        onReleased: () => _halt(_MarioAnimation.climbUp));
    inputHandler.registerKey('DOWN',
        onPressed: () => _climbDown(),
        onReleased: () => _halt(_MarioAnimation.climbDown));
    inputHandler.registerKey('SPACE', onPressed: () => _jump());
  }

  void _walkLeft() {
    if (_state != _MarioAnimation.none) return;
    _state = _MarioAnimation.moveLeft;
    playAnimation(scriptId: _state);
  }

  void _walkRight() {
    if (_state != _MarioAnimation.none) return;
    _state = _MarioAnimation.moveRight;
    playAnimation(scriptId: _state);
  }

  void _climbUp() {
    if (_state != _MarioAnimation.none) return;
    _state = _MarioAnimation.climbUp;
    playAnimation(scriptId: _state);
  }

  void _climbDown() {
    if (_state != _MarioAnimation.none) return;
    _state = _MarioAnimation.climbDown;
    playAnimation(scriptId: _state);
  }

  void _halt(String animation) {
    if (_state != animation) return;

    stopAnimation(scriptId: animation);
    _state = _MarioAnimation.none;
  }

  void _jump() {
    if (_state != _MarioAnimation.none) {
      stopAnimation(scriptId: _state);
    }

    _state = _MarioAnimation.jump;
    playAnimation(
        script: AnimationScript()
          ..id = 'mario_jump'
          ..animation.addAll([
            Animation()
              ..translation = (VectorAnimation()
                ..vec = makeVector([1, -1])
                ..delay = 10
                ..repeat = 25),
            Animation()
              ..translation = (VectorAnimation()
                ..vec = makeVector([1, 1])
                ..delay = 10
                ..repeat = 25),
          ]),
        onDone: (e) => this
          ..gravity = 8
          .._state = _MarioAnimation.none);
    gravity = 0;
  }
}

class _MarioAnimation {
  static const none = '';
  static const moveLeft = 'mario_move_left';
  static const moveRight = 'mario_move_right';
  static const climbUp = 'mario_climb_up';
  static const climbDown = 'mario_climb_down';
  static const jump = 'mario_jump';
}
