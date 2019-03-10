import 'package:dart_troll/dart_troll.dart' as troll;
import 'package:dart_troll/src/proto/action.pb.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';
import 'package:dart_troll/src/proto/primitives.pb.dart';
import 'package:dart_troll/src/proto/scene-node.pb.dart';

/// Sprite representation in Troll engine.
///
/// It defines convenience methods for all operations that can be done on a
/// sprite.
class Sprite {
  String id;
  String spriteId;

  static int _sprite_unique_id = 0;

  Sprite(this.id, this.spriteId) {
    if (id == null) {
      id = spriteId + '_' + (_sprite_unique_id++).toString();
    }
  }

  /// Create a sprite on specified [position] with given [frameIndex].
  void create(List<int> position, int frameIndex) {
    final action = Action()
      ..createSceneNode = (SceneNodeAction()
        ..sceneNode = (SceneNode()
          ..id = id
          ..spriteId = spriteId
          ..frameIndex = frameIndex
          ..position = _makeVector(position)));
    troll.execute(action.writeToBuffer());
  }

  /// Remove the sprite from the scene.
  void destroy() {
    final action = Action()
      ..destroySceneNode =
          (SceneNodeAction()..sceneNode = (SceneNode()..id = id));
    troll.execute(action.writeToBuffer());
  }

  /// Position the sprite at specific coordinates.
  void position(List<int> at) {
    final action = Action()
      ..positionSceneNode = (SceneNodeVectorAction()
        ..sceneNodeId = id
        ..vec = _makeVector(at));
    troll.execute(action.writeToBuffer());
  }

  /// Move the sprite to direction specified by [vec].
  void move(List<int> vec) {
    final action = Action()
      ..moveSceneNode = (SceneNodeVectorAction()
        ..sceneNodeId = id
        ..vec = _makeVector(vec));
    troll.execute(action.writeToBuffer());
  }

  /// Defines consequences, i.e. actions triggered, of the sprite colliding
  /// with other sprite instances or types.
  void onCollision(
      List<String> nodeIds, List<String> spriteIds, List<Action> consequences) {
    final action = Action()
      ..onCollision = (CollisionAction()
        ..sceneNodeId.addAll(nodeIds)
        ..spriteId.addAll(spriteIds)
        ..action.addAll(consequences));
    troll.execute(action.writeToBuffer());
  }

  /// Defines consequences, i.e. actions triggered, of the sprite detaching
  /// with other sprite instances or types.
  void onDetaching(
      List<String> nodeIds, List<String> spriteIds, List<Action> consequences) {
    final action = Action()
      ..onDetaching = (CollisionAction()
        ..sceneNodeId.addAll(nodeIds)
        ..spriteId.addAll(spriteIds)
        ..action.addAll(consequences));
    troll.execute(action.writeToBuffer());
  }

  /// Play an animation script on the sprite.
  ///
  ///  If [onDone] is provided it will be invoked when the script finishes
  ///  execution.
  void playAnimationScript(AnimationScript script, Function onDone) {
    final action = Action()
      ..playAnimationScript = (AnimationScriptAction()
        ..script = script
        ..sceneNodeId = id);
    troll.execute(action.writeToBuffer());

    // TODO(bourdenas): Implement onDone when events are available in Dart.
  }

  /// Look up an animation script from resources and play it on the sprite.
  ///
  /// If  [onDone] is provided it will be invoked when the script finishes
  /// execution.
  void playAnimationScriptById(String scriptId, Function onDone) {
    final action = Action()
      ..playAnimationScript = (AnimationScriptAction()
        ..scriptId = scriptId
        ..sceneNodeId = id);
    troll.execute(action.writeToBuffer());

    // TODO(bourdenas): Implement onDone when events are available in Dart.
  }

  /// Stop an active animation script on the sprite.
  ///
  /// If [onDone] was provided during [playAnimationSript], it will be invoked.
  void stopAnimationScript(String scriptId) {
    final action = Action()
      ..stopAnimationScript = (AnimationScriptAction()
        ..scriptId = scriptId
        ..sceneNodeId = id);
    troll.execute(action.writeToBuffer());
  }

  /// Puase an active animation script on the sprite.
  ///
  /// If [onDone] was provided during [playAnimationSript], it will be invoked.
  ///
  /// TODO(bourdenas): Allow resume with [playAnimationScriptById].
  void pauseAnimationScript(String scriptId) {
    final action = Action()
      ..stopAnimationScript = (AnimationScriptAction()
        ..scriptId = scriptId
        ..sceneNodeId = id);
    troll.execute(action.writeToBuffer());
  }
}

/// Returns a Vector from input list.
///
/// Throws [ArgumentError] if the list does not have proper size (2 or 3).
/// Treats the third dimension as optional.
Vector _makeVector(List<int> list) {
  if (list.length < 2 || list.length > 3) {
    throw ArgumentError('list size for position have length 2 or 3');
  }

  var vec = Vector()
    ..x = list[0].toDouble()
    ..y = list[1].toDouble();

  if (list.length == 3) vec.z = list[2].toDouble();
  return vec;
}
