import 'package:dart_troll/src/core/sprite.dart';

class Princess extends Sprite {
  Princess(List<int> position, {int frameIndex = 0}) : super('princess') {
    create(position, frameIndex: frameIndex);
  }
}
