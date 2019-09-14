import 'package:dart_dk/src/sprites/gravity_sprite.dart';
import 'package:dart_troll/src/core/sprite.dart';
import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';

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
