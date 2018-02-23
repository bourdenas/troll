import proto.action_pb2
import troll


class TrollElement:
  def __init__(self, id, sprite_id):
    self.id = id
    self.sprite_id = sprite_id

  def Create(self, frame_index, position):
    troll.execute(self.CreateAction(frame_index, position).SerializeToString())

  def CreateAction(self, frame_index, position):
    action = proto.action_pb2.Action()
    action.create_scene_node.scene_node.id = self.id
    action.create_scene_node.scene_node.sprite_id = self.sprite_id
    action.create_scene_node.scene_node.frame_index = frame_index
    action.create_scene_node.scene_node.position.x = position[0]
    action.create_scene_node.scene_node.position.y = position[1]
    return action

  def Destroy(self):
    troll.execute(self.DestroyAction().SerializeToString())

  def DestroyAction(self):
    action = proto.action_pb2.Action()
    action.destroy_scene_node.scene_node.id = self.id
    return action

  def PlayAnimation(self, script_id):
    troll.execute(self.PlayAnimationAction(script_id).SerializeToString())

  def PlayAnimationAction(self, script_id):
    action = proto.action_pb2.Action()
    action.play_animation_script.scene_node_id = self.id
    action.play_animation_script.script_id = script_id
    return action

  def StopAnimation(self, script_id=""):
    troll.execute(self.StopAnimationAction(script_id).SerializeToString())

  def StopAnimationAction(self, script_id=""):
    action = proto.action_pb2.Action()
    action.stop_animation_script.scene_node_id = self.id
    action.stop_animation_script.script_id = script_id
    return action

  def PauseAnimation(self, script_id=""):
    troll.execute(self.PauseAnimationAction(script_id).SerializeToString())

  def PauseAnimation(self, script_id=""):
    action = proto.action_pb2.Action()
    action.pause_animation_script.scene_node_id = self.id
    action.pause_animation_script.script_id = script_id
    return action
