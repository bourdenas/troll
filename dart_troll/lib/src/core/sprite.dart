import 'package:dart_troll/dart_troll.dart' as troll;
import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/proto/action.pb.dart';
import 'package:dart_troll/src/proto/query.pb.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';
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
    id ??= spriteId + '_' + (_sprite_unique_id++).toString();
  }

  /// Create a sprite on specified [position] with given [frameIndex].
  void create(List<int> position, {int frameIndex = 0}) {
    final action = Action()
      ..createSceneNode = (SceneNodeAction()
        ..sceneNode = (SceneNode()
          ..id = id
          ..spriteId = spriteId
          ..frameIndex = frameIndex
          ..position = makeVector(position)));
    troll.execute(action.writeToBuffer());
  }

  /// Remove the sprite from the scene.
  void destroy() {
    final action = Action()
      ..destroySceneNode =
          (SceneNodeAction()..sceneNode = (SceneNode()..id = id));
    troll.execute(action.writeToBuffer());
  }

  void set position(List<int> at) {
    final action = Action()
      ..positionSceneNode = (SceneNodeVectorAction()
        ..sceneNodeId = id
        ..vec = makeVector(at));
    troll.execute(action.writeToBuffer());
  }

  List<int> get position {
    final query = Query()
      ..sceneNode = (SceneNodeQuery()..pattern = (SceneNode()..id = id));
    final responseBuffer = troll.eval(query.writeToBuffer());
    final response = Response()..mergeFromBuffer(responseBuffer);
    final node = response.sceneNodes.sceneNode[0];
    return [
      node.position.x.toInt(),
      node.position.y.toInt(),
      node.position.z.toInt()
    ];
  }

  /// Move the sprite to direction specified by [vec].
  void move(List<int> vec) {
    final action = Action()
      ..moveSceneNode = (SceneNodeVectorAction()
        ..sceneNodeId = id
        ..vec = makeVector(vec));
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
  void playAnimationScript(AnimationScript script,
      {Function onDone, Function onRewind, Map<String, Function> onPartDone}) {
    final action = Action()
      ..playAnimationScript = (AnimationScriptAction()
        ..script = script
        ..sceneNodeId = id);
    troll.execute(action.writeToBuffer());

    if (onDone != null) {
      onScriptDone(script.id, onDone);
    }
    if (onRewind != null) {
      onScriptRewind(script.id, onRewind);
    }
    if (onPartDone != null) {
      onScriptPartDone(script.id, onPartDone);
    }
  }

  /// Look up an animation script from resources and play it on the sprite.
  ///
  /// If  [onDone] is provided it will be invoked when the script finishes
  /// execution.
  void playAnimationScriptById(String scriptId,
      {Function onDone, Function onRewind, Map<String, Function> onPartDone}) {
    final action = Action()
      ..playAnimationScript = (AnimationScriptAction()
        ..scriptId = scriptId
        ..sceneNodeId = id);
    troll.execute(action.writeToBuffer());

    if (onDone != null) {
      onScriptDone(scriptId, onDone);
    }
    if (onRewind != null) {
      onScriptRewind(scriptId, onRewind);
    }
    if (onPartDone != null) {
      onScriptPartDone(scriptId, onPartDone);
    }
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
  /// TODO(bourdenas): Allow resume with [playAnimationScriptById].
  void pauseAnimationScript(String scriptId) {
    final action = Action()
      ..stopAnimationScript = (AnimationScriptAction()
        ..scriptId = scriptId
        ..sceneNodeId = id);
    troll.execute(action.writeToBuffer());
  }

  /// Invokes [callback] when [scriptId] on sprite finishes.
  void onScriptDone(String scriptId, Function callback) {
    troll.registerEventHandler(id + '.' + scriptId + '.done', callback);
  }

  /// Invokes [callback] when [scriptId] on sprite rewinds (finishes and
  /// starts again).
  void onScriptRewind(String scriptId, Function callback) {
    troll.registerEventHandler(id + '.' + scriptId + '.rewind', callback,
        permanent: true);
  }

  /// Invokes associatied callbacks when parts of [scriptId] on sprite finish.
  void onScriptPartDone(String scriptId, Map<String, Function> callbacks) {
    callbacks.forEach((part, callback) {
      troll.registerEventHandler(
          id + '.' + scriptId + '.' + part + '.done', callback,
          permanent: true);
    });
  }
}
