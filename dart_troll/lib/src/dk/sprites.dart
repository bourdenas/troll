import 'package:dart_troll/src/core/sprite.dart';

class DonkeyKong extends Sprite {
  DonkeyKong() : super(null, 'dk');
}

class PlatformPiece extends Sprite {
  PlatformPiece() : super(null, 'platform') {}
}

class Platform {
  int size;
  List<PlatformPiece> pieces;

  static int platformWidth = 32;

  Platform(this.size) {
    pieces = List<PlatformPiece>.generate(size, (index) {
      return PlatformPiece();
    });
  }

  void create(List<int> position) {
    pieces.forEach((piece) {
      position[0] += platformWidth;
      piece.create(position, 0);
    });
  }
}

class Ladder extends Sprite {
  Ladder() : super(null, 'ladder');
}
