import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/core/sprite.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';

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

class Princess extends Sprite {
  Princess(List<int> position, {int frameIndex = 0}) : super('princess') {
    create(position, frameIndex: frameIndex);
  }
}

class _PlatformPiece extends Sprite {
  _PlatformPiece(List<int> position) : super('platform') {
    create(position);
  }
}

class Platform {
  int _index;
  int _size;
  List<_PlatformPiece> _pieces;

  static int _platformWidth = 32;

  Platform(this._index, this._size, List<int> position) {
    _pieces = List<_PlatformPiece>.generate(_size, (_index) {
      position[0] += _platformWidth;
      return _PlatformPiece(position);
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

class _PlatformSpec {
  int size;
  List<int> position;

  _PlatformSpec(this.size, this.position);
}

class Scaffold {
  final _platformSpecs = [
    _PlatformSpec(3, [230, 97, -3]),
    _PlatformSpec(13, [80, 155, -3]),
    _PlatformSpec(13, [112, 210, -3]),
    _PlatformSpec(13, [80, 275, -3]),
    _PlatformSpec(13, [112, 335, -3]),
    _PlatformSpec(13, [80, 395, -3]),
    _PlatformSpec(14, [70, 455, -3]),
  ];
  final _ladderPositions = [
    [210, 72, -1],
    [242, 72, -1],
    [210, 112, -1],
    [242, 112, -1],
    [338, 112, -1],
  ];

  List<Platform> _platforms;

  Scaffold() {
    _platforms = _platformSpecs
        .asMap()
        .map((index, spec) =>
            MapEntry(index, Platform(index, spec.size, spec.position)))
        .values
        .toList();

    for (var pos in _ladderPositions) {
      Ladder(pos);
    }
  }

  void collapsePlatform(int index) {
    _platforms[index].collapse();
  }

  void buildStage() {
    for (final platform in _platforms.sublist(1)) {
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
    for (final pos in brokenLadderPositions) {
      Ladder(pos, frameIndex: 1);
    }

    final ladderPositions = [
      [444, 420, -5],
      [280, 358, -5],
      [148, 353, -5],
      [310, 297, -5],
      [444, 294, -5],
      [148, 235, -5],
      [226, 235, -5],
      [444, 172, -5],
    ];
    for (final pos in ladderPositions) {
      Ladder(pos);
    }

    Barrel([80, 88], frameIndex: 5);
    Barrel([100, 428], frameIndex: 6);
  }
}

class Ladder extends Sprite {
  Ladder(List<int> position, {int frameIndex = 0}) : super('ladder') {
    create(position, frameIndex: frameIndex);
  }
}

class RollingLadder {
  final _positions = [
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

  List<Ladder> _ladders;

  RollingLadder() {
    _ladders = [for (var pos in _positions.reversed) Ladder(pos)];

    final ladderFadingScript = AnimationScript()
      ..animation.add(Animation()..flash = (FlashAnimation()..repeat = 1));
    for (var i = 0; i < 4; ++i) {
      ladderFadingScript.animation[0].flash.delay = 700 * (i + 1);
      _ladders[i * 2].playAnimation(script: ladderFadingScript);
      _ladders[i * 2 + 1].playAnimation(script: ladderFadingScript);
    }
  }

  void destroyTopSegment() {
    for (int i = 4; i < 7; ++i) {
      _ladders[i * 2].destroy();
      _ladders[i * 2 + 1].destroy();
    }
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
