import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/core/sprite.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';

class Mario extends Sprite {
  Mario() : super(null, 'mario');
}

class DonkeyKong extends Sprite {
  DonkeyKong() : super(null, 'dk');
}

class Princess extends Sprite {
  Princess() : super(null, 'princess');
}

class PlatformPiece extends Sprite {
  PlatformPiece() : super(null, 'platform') {}
}

class Platform {
  int index;
  int size;
  List<PlatformPiece> pieces;

  static int platformWidth = 32;

  Platform(this.index, this.size) {
    pieces = List<PlatformPiece>.generate(size, (index) => PlatformPiece());
  }

  void create(List<int> position) {
    for (final piece in pieces) {
      position[0] += platformWidth;
      piece.create(position);
    }
  }

  void collapse() {
    if (index == 1) {
      for (int i = 0; i < 4; ++i) {
        pieces[pieces.length - i - 1].move([0, 4 - i]);
      }
    } else if (index == 6) {
      for (int i = 0; i < 7; ++i) {
        pieces[pieces.length - i - 1].move([0, i]);
        pieces[i].move([0, 7]);
      }
    } else if (index % 2 == 0) {
      pieces.asMap().forEach((i, piece) {
        piece.move([0, pieces.length - i - 1]);
      });
    } else if (index % 2 == 1) {
      pieces.asMap().forEach((i, piece) {
        piece.move([0, i]);
      });
    }
  }
}

class Ladder extends Sprite {
  Ladder() : super(null, 'ladder');
}

class Barrel extends Sprite {
  Barrel() : super(null, 'barrel');

  void Roll() {
    final script = AnimationScript()
      ..id = 'barrel_roll'
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
