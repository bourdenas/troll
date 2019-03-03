import proto.action_pb2


def __assign_vec(proto_vec, py_tuple):
    proto_vec.x, proto_vec.y, proto_vec.z = py_tuple


def ChangeScene(scene):
    action = proto.action_pb2.Action()
    action.change_scene.scene.CopyFrom(scene)
    return action


def Create(node_id, sprite_id, frame_index, position=(0, 0, 0,)):
    action = proto.action_pb2.Action()
    action.create_scene_node.scene_node.id = node_id
    action.create_scene_node.scene_node.sprite_id = sprite_id
    action.create_scene_node.scene_node.frame_index = frame_index
    __assign_vec(action.create_scene_node.scene_node.position, position)
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


def PlayAudio(track_id='', sfx_id='', repeat=1):
    action = proto.action_pb2.Action()
    if track_id:
        action.play_audio.track_id = track_id
    elif sfx_id:
        action.play_audio.sfx_id = sfx_id
    action.play_audio.repeat = repeat
    return action


def StopAudio(sfx_id=''):
    action = proto.action_pb2.Action()
    if sfx_id:
        action.stop_audio.sfx_id = sfx_id
    else:
        action.stop_audio.track_id = ''
    return action


def PauseAudio(sfx_id=''):
    action = proto.action_pb2.Action()
    if sfx_id:
        action.pause_audio.sfx_id = sfx_id
    else:
        action.pause_audio.track_id = ''
    return action


def ResumeAudio(sfx_id=''):
    action = proto.action_pb2.Action()
    if sfx_id:
        action.resume_audio.sfx_id = sfx_id
    else:
        action.resume_audio.track_id = ''
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
