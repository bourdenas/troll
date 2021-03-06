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
typedef CollisionHandler = void Function(
    String eventId, Iterable<Sprite> sprites);

/// Sprite representation in Troll engine.
///
/// It defines convenience methods for operations that can be done on a sprite.
class Sprite {
  String nodeId;
  String spriteId;

  static int _sprite_unique_id = 0;
  int _sprite_event_id = 0;

  Sprite(this.spriteId, [this.nodeId]) {
    nodeId ??= spriteId + '_' + (_sprite_unique_id++).toString();
  }

  Sprite.wrap(this.nodeId);

  /// Create a sprite on specified [position] with given [frameIndex].
  void create(List<int> position, {int frameIndex = 0}) {
    final action = Action()
      ..createSceneNode = (SceneNodeAction()
        ..sceneNode = (SceneNode()
          ..id = nodeId
          ..spriteId = spriteId
          ..frameIndex = frameIndex
          ..position = makeVector(position)));
    troll.execute(action.writeToBuffer());
  }

  /// Remove the sprite from the scene.
  void destroy() {
    final action = Action()
      ..destroySceneNode =
          (SceneNodeAction()..sceneNode = (SceneNode()..id = nodeId));
    troll.execute(action.writeToBuffer());
  }

  void set position(List<int> at) {
    final action = Action()
      ..positionSceneNode = (SceneNodeVectorAction()
        ..sceneNodeId = nodeId
        ..vec = makeVector(at));
    troll.execute(action.writeToBuffer());
  }

  List<int> get position {
    final query = Query()
      ..sceneNode = (SceneNodeQuery()..pattern = (SceneNode()..id = nodeId));
    final responseBuffer = troll.eval(query.writeToBuffer());
    final response = Response()..mergeFromBuffer(responseBuffer);
    final node = response.sceneNodes.sceneNode[0];
    return [
      node.position.x.toInt(),
      node.position.y.toInt(),
      node.position.z.toInt()
    ];
  }

  void set frameIndex(int index) {
    final action = Action()
      ..playAnimationScript = (AnimationScriptAction()
        ..sceneNodeId = nodeId
        ..script = (AnimationScript()
          ..id = 'set_frame_{$nodeId}_{$index}'
          ..animation.addAll([
            Animation()
              ..frameList = (FrameListAnimation()
                ..frame.add(index)
                ..repeat = 1),
          ])));
    troll.execute(action.writeToBuffer());
  }

  int get frameIndex {
    final query = Query()
      ..sceneNode = (SceneNodeQuery()..pattern = (SceneNode()..id = nodeId));
    final responseBuffer = troll.eval(query.writeToBuffer());
    final response = Response()..mergeFromBuffer(responseBuffer);
    final node = response.sceneNodes.sceneNode[0];
    return node.frameIndex;
  }

  /// Move the sprite to direction specified by [vec].
  void move(List<int> vec) {
    final action = Action()
      ..moveSceneNode = (SceneNodeVectorAction()
        ..sceneNodeId = nodeId
        ..vec = makeVector(vec));
    troll.execute(action.writeToBuffer());
  }

  /// Registers an [handler] that is triggered when this sprite collide with
  /// other sprites.
  ///
  /// At least one of [nodeId] and [spriteId] needs to be provideds.
  void onCollision({String nodeId, String spriteId, CollisionHandler handler}) {
    final action = Action()..onCollision = CollisionAction();
    _buildCollisionAction(action.onCollision, nodeId, spriteId, handler);
    troll.execute(action.writeToBuffer());
  }

  /// Registers an [handler] that is triggered when this sprite overlaps with
  /// other sprites.
  ///
  /// At least one of [nodeId] and [spriteId] needs to be provideds.
  /// In contrast to [onCollision] that triggers only when sprites collide this
  /// event is triggered constantly as long as the sprites overlap.
  void onOverlap({String nodeId, String spriteId, CollisionHandler handler}) {
    final action = Action()..onOverlap = CollisionAction();
    _buildCollisionAction(action.onOverlap, nodeId, spriteId, handler);
    troll.execute(action.writeToBuffer());
  }

  /// Registers an [handler] that is triggered when this sprite detaches from
  /// other sprites.
  ///
  /// At least one of [nodeId] and [spriteId] needs to be provideds.
  void onDetaching({String nodeId, String spriteId, CollisionHandler handler}) {
    final action = Action()..onDetaching = CollisionAction();
    _buildCollisionAction(action.onDetaching, nodeId, spriteId, handler);
    troll.execute(action.writeToBuffer());
  }

  /// Builds a collision action that will trigger the input [CollisionHandler].
  ///
  /// An EmitAction is used in the CollisionAction to generate unique events
  /// that trigger the [handler].
  void _buildCollisionAction(CollisionAction collisionAction, String nodeId,
      String spriteId, CollisionHandler handler) {
    collisionAction.sceneNodeId
        .addAll([this.nodeId, if (nodeId != null) nodeId]);
    if (spriteId != null) collisionAction.spriteId.add(spriteId);

    final eventId = this.nodeId + '_' + (_sprite_event_id++).toString();
    final nodePattern = [
      if (nodeId != null) 'node_id: "$nodeId"',
      if (spriteId != null) 'sprite_id: "$spriteId"',
    ].join(' ');

    collisionAction.action.add(Action()
      ..emit = (EmitAction()
        ..event = (Event()..eventId = eventId)
        ..sceneNodePattern = '\$this.{$nodePattern}'));

    troll.registerEventHandler(
        eventId,
        (Uint8List eventBuffer) =>
            _onCollisionEvent(Event()..mergeFromBuffer(eventBuffer), handler),
        permanent: true);
  }

  /// Converts a collision event into arguments for a CollisionHandler to
  /// provide a type-safe API for handling collision type events.
  static void _onCollisionEvent(Event e, CollisionHandler handler) {
    handler(
        e.eventId, [for (final nodeId in e.sceneNodeId) Sprite.wrap(nodeId)]);
  }

  /// Returns a box that describes the overlap of this sprite with the one
  /// described by [sceneNodeId].
  Box getOverlap(String sceneNodeId) {
    final query = Query()
      ..sceneNodeOverlap = (SceneNodePairQuery()
        ..firstNodeId = nodeId
        ..secondNodeId = sceneNodeId);
    final responseBuffer = troll.eval(query.writeToBuffer());
    final response = Response()..mergeFromBuffer(responseBuffer);
    return response.overlap;
  }

  /// Plays an animation on the sprite. The animation can be the provided
  /// [script] or an animation from resources based on [scriptId].
  ///
  /// If [onDone] is provided it will be invoked when the script finishes
  /// execution.
  /// If [onRewind] is provided it will be invoked when the script restarts
  /// execution.
  /// The [onPartDone] is a map from animation script part name to handlers. If
  /// provided a handler will be invoked when the corresponding script part
  /// finishes execution.
  void playAnimation({
    AnimationScript script,
    String scriptId,
    EventHandler onDone,
    EventHandler onRewind,
    Map<String, EventHandler> onPartDone,
  }) {
    final animationScript = AnimationScriptAction()..sceneNodeId = nodeId;
    if (script != null) animationScript..script = script;
    if (scriptId != null) animationScript..scriptId = scriptId;

    final action = Action()..playAnimationScript = animationScript;
    troll.execute(action.writeToBuffer());

    final validId = script != null ? script.id : scriptId;
    if (onDone != null) {
      onScriptDone(validId, onDone);
    }
    if (onRewind != null) {
      onScriptRewind(validId, onRewind);
    }
    if (onPartDone != null) {
      onScriptPartDone(validId, onPartDone);
    }
  }

  /// Stops an active animation script on the sprite.
  ///
  /// If [onDone] was provided during [playAnimationSript], it will be invoked.
  void stopAnimation({String scriptId}) {
    final animationScript = AnimationScriptAction()..sceneNodeId = nodeId;
    if (scriptId != null) animationScript..scriptId = scriptId;

    final action = Action()..stopAnimationScript = animationScript;
    troll.execute(action.writeToBuffer());
  }

  /// Pauses an active animation script on the sprite.
  void pauseAnimation({String scriptId}) {
    final animationScript = AnimationScriptAction()..sceneNodeId = nodeId;
    if (scriptId != null) animationScript..scriptId = scriptId;

    final action = Action()..pauseAnimationScript = animationScript;
    troll.execute(action.writeToBuffer());
  }

  /// Resumes an animation script on the sprite that was paused before.
  void resumeAnimation({String scriptId}) {
    final animationScript = AnimationScriptAction()..sceneNodeId = nodeId;
    if (scriptId != null) animationScript..scriptId = scriptId;

    final action = Action()..playAnimationScript = animationScript;
    troll.execute(action.writeToBuffer());
  }

  /// Invokes [callback] when [scriptId] on sprite finishes.
  void onScriptDone(String scriptId, EventHandler callback) {
    troll.registerEventHandler(
        nodeId + '.' + scriptId + '.done',
        (Uint8List eventBuffer) =>
            callback(Event()..mergeFromBuffer(eventBuffer)));
  }

  /// Invokes [callback] when [scriptId] on sprite rewinds (finishes and
  /// starts again).
  void onScriptRewind(String scriptId, EventHandler callback) {
    troll.registerEventHandler(
        nodeId + '.' + scriptId + '.rewind',
        (Uint8List eventBuffer) =>
            callback(Event()..mergeFromBuffer(eventBuffer)),
        permanent: true);
  }

  /// Invokes associatied callbacks when parts of [scriptId] on sprite finish.
  void onScriptPartDone(String scriptId, Map<String, EventHandler> callbacks) {
    callbacks.forEach((part, callback) {
      troll.registerEventHandler(
          nodeId + '.' + scriptId + '.' + part + '.done',
          (Uint8List eventBuffer) =>
              callback(Event()..mergeFromBuffer(eventBuffer)),
          permanent: true);
    });
  }
}
