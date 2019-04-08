import 'dart:typed_data';

import 'package:dart_troll/dart_troll.dart' as troll;
import 'package:dart_troll/src/core/util.dart';
import 'package:dart_troll/src/proto/action.pb.dart';
import 'package:dart_troll/src/proto/animation.pb.dart';
import 'package:dart_troll/src/proto/event.pb.dart';
import 'package:dart_troll/src/proto/primitives.pb.dart';
import 'package:dart_troll/src/proto/query.pb.dart';
import 'package:dart_troll/src/proto/scene-node.pb.dart';

typedef EventHandler = void Function(Event);

/// Sprite representation in Troll engine.
///
/// It defines convenience methods for all operations that can be done on a
/// sprite.
class Sprite {
  String id;
  String spriteId;

  static int _sprite_unique_id = 0;
  int _sprite_event_id = 0;

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

  /// Registers consequences, i.e. actions triggered, of the sprite colliding
  /// with other sprites.
  ///
  /// At least one of {[nodeId], [spriteId]} and one of
  /// {[eventHandler], [actions]} needs to be provideds.
  void onCollision(
      {String nodeId,
      String spriteId,
      EventHandler eventHandler,
      List<Action> actions}) {
    final action = Action()
      ..onCollision = (CollisionAction()..sceneNodeId.add(id));
    _buildCollisionAction(
        action.onCollision, nodeId, spriteId, eventHandler, actions);
    troll.execute(action.writeToBuffer());
  }

  /// Registers consequences, i.e. actions triggered, of the sprite detaching
  /// with other sprites.
  ///
  /// At least one of {[nodeId], [spriteId]} and one of
  /// {[eventHandler], [actions]} needs to be provideds.
  void onDetaching(
      {String nodeId,
      String spriteId,
      EventHandler eventHandler,
      List<Action> actions}) {
    final action = Action()
      ..onDetaching = (CollisionAction()..sceneNodeId.add(id));
    _buildCollisionAction(
        action.onDetaching, nodeId, spriteId, eventHandler, actions);
    troll.execute(action.writeToBuffer());
  }

  /// Builds a collision action in order to support EventHandlers mechanism.
  void _buildCollisionAction(CollisionAction collision, String nodeId,
      String spriteId, EventHandler eventHandler, List<Action> actions) {
    if (nodeId != null) {
      collision.sceneNodeId.add(nodeId);
    }
    if (spriteId != null) {
      collision.spriteId.add(spriteId);
    }

    if (eventHandler != null) {
      final eventId = id + '_' + (_sprite_event_id++).toString();
      var nodePattern = '';
      if (nodeId != null) nodePattern += 'node_id: "$nodeId" ';
      if (spriteId != null) nodePattern += 'sprite_id: "$spriteId" ';
      collision.action.add(Action()
        ..emit = (EmitAction()
          ..event = (Event()..eventId = eventId)
          ..sceneNodePattern = '\$this.{$nodePattern}'));
      troll.registerEventHandler(
          eventId,
          (Uint8List eventBuffer) =>
              eventHandler(Event()..mergeFromBuffer(eventBuffer)),
          permanent: true);
    }
    if (actions != null) {
      collision.action.addAll(actions);
    }
  }

  Box getOverlap(String sceneNodeId) {
    final query = Query()
      ..sceneNodeOverlap = (SceneNodePairQuery()
        ..firstNodeId = id
        ..secondNodeId = sceneNodeId);
    final responseBuffer = troll.eval(query.writeToBuffer());
    final response = Response()..mergeFromBuffer(responseBuffer);
    return response.overlap;
  }

  /// Play an animation script on the sprite.
  ///
  ///  If [onDone] is provided it will be invoked when the script finishes
  ///  execution.
  void playAnimationScript(AnimationScript script,
      {EventHandler onDone,
      EventHandler onRewind,
      Map<String, EventHandler> onPartDone}) {
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
      {EventHandler onDone,
      EventHandler onRewind,
      Map<String, EventHandler> onPartDone}) {
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
  void onScriptDone(String scriptId, EventHandler callback) {
    troll.registerEventHandler(
        id + '.' + scriptId + '.done',
        (Uint8List eventBuffer) =>
            callback(Event()..mergeFromBuffer(eventBuffer)));
  }

  /// Invokes [callback] when [scriptId] on sprite rewinds (finishes and
  /// starts again).
  void onScriptRewind(String scriptId, EventHandler callback) {
    troll.registerEventHandler(
        id + '.' + scriptId + '.rewind',
        (Uint8List eventBuffer) =>
            callback(Event()..mergeFromBuffer(eventBuffer)),
        permanent: true);
  }

  /// Invokes associatied callbacks when parts of [scriptId] on sprite finish.
  void onScriptPartDone(String scriptId, Map<String, EventHandler> callbacks) {
    callbacks.forEach((part, callback) {
      troll.registerEventHandler(
          id + '.' + scriptId + '.' + part + '.done',
          (Uint8List eventBuffer) =>
              callback(Event()..mergeFromBuffer(eventBuffer)),
          permanent: true);
    });
  }
}
