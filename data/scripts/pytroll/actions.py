import proto.action_pb2


def Create(node_id, sprite_id, frame_index, position):
    action = proto.action_pb2.Action()
    action.create_scene_node.scene_node.id = node_id
    action.create_scene_node.scene_node.sprite_id = sprite_id
    action.create_scene_node.scene_node.frame_index = frame_index
    action.create_scene_node.scene_node.position.x = position[0]
    action.create_scene_node.scene_node.position.y = position[1]
    action.create_scene_node.scene_node.position.z = position[2]
    return action


def Destroy(node_id):
    action = proto.action_pb2.Action()
    action.destroy_scene_node.scene_node.id = node_id
    return action


def Position(node_id, position):
    action = proto.action_pb2.Action()
    action.position_scene_node.scene_node_id = node_id
    __assign_vec(action.position_scene_node.vec, position)
    return action


def Move(node_id, vec):
    action = proto.action_pb2.Action()
    action.move_scene_node.scene_node_id = node_id
    __assign_vec(action.move_scene_node.vec, vec)
    return action


def PlayAnimationScript(node_id, script):
    action = proto.action_pb2.Action()
    action.play_animation_script.scene_node_id = node_id
    action.play_animation_script.script.CopyFrom(script)
    return action


def PlayAnimation(node_id, script_id):
    action = proto.action_pb2.Action()
    action.play_animation_script.scene_node_id = node_id
    action.play_animation_script.script_id = script_id
    return action


def StopAnimation(node_id, script_id=""):
    action = proto.action_pb2.Action()
    action.stop_animation_script.scene_node_id = node_id
    action.stop_animation_script.script_id = script_id
    return action


def PauseAnimation(node_id, script_id=""):
    action = proto.action_pb2.Action()
    action.pause_animation_script.scene_node_id = node_id
    action.pause_animation_script.script_id = script_id
    return action


def OnCollision(node_ids, sprite_ids, actions):
    action = proto.action_pb2.Action()
    if node_ids:
        action.on_collision.scene_node_id.extend(node_ids)
    if sprite_ids:
        action.on_collision.sprite_id.extend(sprite_ids)
    action.on_collision.action.extend(actions)
    return action


def OnDetaching(node_ids, sprite_ids, actions):
    action = proto.action_pb2.Action()
    if node_ids:
        action.on_detaching.scene_node_id.extend(node_ids)
    if sprite_ids:
        action.on_detaching.sprite_id.extend(sprite_ids)
    action.on_detaching.action.extend(actions)
    return action


def Call(module, function):
    action = proto.action_pb2.Action()
    action.call.module = module
    action.call.function = function
    return action
