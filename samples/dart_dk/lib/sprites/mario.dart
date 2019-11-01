import 'package:dart_troll/src/core/audio.dart';
import 'package:dart_troll/src/core/input_handler.dart';
import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';

import 'package:dart_dk/src/gravity_sprite.dart';

class Mario extends GavitySprite {
  var _runningAnimations = <String>{};

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
        onReleased: () => _halt(_MarioAnimation.moveLeft, haltFrameIndex: 0));
    inputHandler.registerKey('RIGHT',
        onPressed: () => _walkRight(),
        onReleased: () => _halt(_MarioAnimation.moveRight, haltFrameIndex: 15));
    inputHandler.registerKey('UP',
        onPressed: () => _climbUp(),
        onReleased: () => _halt(_MarioAnimation.climbUp));
    inputHandler.registerKey('DOWN',
        onPressed: () => _climbDown(),
        onReleased: () => _halt(_MarioAnimation.climbDown));
    inputHandler.registerKey('SPACE', onPressed: () => _jump());
  }

  void _walkLeft() {
    if (_runningAnimations.isNotEmpty) return;
    _runningAnimations.add(_MarioAnimation.moveLeft);
    playAnimation(scriptId: _MarioAnimation.moveLeft);
  }

  void _walkRight() {
    if (_runningAnimations.isNotEmpty) return;
    _runningAnimations.add(_MarioAnimation.moveRight);
    playAnimation(scriptId: _MarioAnimation.moveRight);
  }

  void _climbUp() {
    if (_runningAnimations.isNotEmpty) return;
    _runningAnimations.add(_MarioAnimation.climbUp);
    playAnimation(scriptId: _MarioAnimation.climbUp);
  }

  void _climbDown() {
    if (_runningAnimations.isNotEmpty) return;
    _runningAnimations.add(_MarioAnimation.climbDown);
    playAnimation(scriptId: _MarioAnimation.climbDown);
  }

  void _halt(String animation, {int haltFrameIndex}) {
    _runningAnimations.remove(animation);
    stopAnimation(scriptId: animation);

    if (_runningAnimations.isEmpty && haltFrameIndex != null) {
      frameIndex = haltFrameIndex;
    }
  }

  void _jump() {
    if (_runningAnimations.intersection({
      _MarioAnimation.climbUp,
      _MarioAnimation.climbDown,
      _MarioAnimation.jump,
    }).isNotEmpty) {
      return;
    }

    for (final animation in _runningAnimations) {
      pauseAnimation(scriptId: animation);
    }

    final direction = frameIndex < 3 ? -1 : 1;
    frameIndex = direction == 1 ? 16 : 1;
    gravity = 0;

    _runningAnimations.add(_MarioAnimation.jump);
    playAnimation(
        script: AnimationScript()
          ..id = 'mario_jump'
          ..animation.addAll([
            Animation()
              ..translation = (VectorAnimation()
                ..vec = makeVector([direction, -1])
                ..delay = 10
                ..repeat = 25),
            Animation()
              ..translation = (VectorAnimation()
                ..vec = makeVector([direction, 1])
                ..delay = 10
                ..repeat = 25),
          ]),
        onDone: (e) {
          this
            ..gravity = 8
            ..frameIndex = frameIndex - 1
            .._runningAnimations.remove(_MarioAnimation.jump);

          for (final animation in _runningAnimations) {
            resumeAnimation(scriptId: animation);
          }
        });
    playSfx('jump');
  }
}

class _MarioAnimation {
  static const moveLeft = 'mario_move_left';
  static const moveRight = 'mario_move_right';
  static const climbUp = 'mario_climb_up';
  static const climbDown = 'mario_climb_down';
  static const jump = 'mario_jump';
}
