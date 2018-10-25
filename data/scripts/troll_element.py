import actions
import troll


class Sprite:
    def __init__(self, id, sprite_id):
        self.id = id
        self.sprite_id = sprite_id

    def Create(self, position, frame_index=0):
        action = actions.Create(self.id, self.sprite_id, frame_index, position)
        troll.execute(action.SerializeToString())

    def Destroy(self):
        action = actions.Destroy(self.id)
        troll.execute(action.SerializeToString())

    def OnCollision(self, node_id='', sprite_id='', on_collide=[]):
        action = actions.OnCollision(
            [self.id, node_id] if node_id else [self.id],
            [sprite_id] if sprite_id else [],
            on_collide)
        troll.execute(action.SerializeToString())

    def OnDetaching(self, node_id='', sprite_id='', on_detach=[]):
        action = actions.OnDetaching(
            [self.id, node_id] if node_id else [self.id],
            [sprite_id] if sprite_id else [],
            on_detach)
        troll.execute(action.SerializeToString())

    def PlayAnimation(self, script_id):
        action = actions.PlayAnimation(self.id, script_id)
        troll.execute(action.SerializeToString())

    def StopAnimation(self, script_id=""):
        action = actions.StopAnimation(self.id, script_id)
        troll.execute(action.SerializeToString())

    def PauseAnimation(self, script_id=""):
        action = actions.PauseAnimation(self.id, script_id)
        troll.execute(action.SerializeToString())
