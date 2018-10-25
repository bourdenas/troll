import actions
import troll


class Sprite:
    def __init__(self, id, sprite_id):
        self.id = id
        self.sprite_id = sprite_id

    def Create(self, position, frame_index=0):
        troll.execute(actions.Create(
            self.id, self.sprite_id, frame_index, position))

    def Destroy(self):
        troll.execute(actions.Destroy(self.id))

    def OnCollision(self, node_id='', sprite_id='', on_collide=[]):
        troll.execute(actions.OnCollision(
            [self.id, node_id] if node_id else [self.id],
            [sprite_id] if sprite_id else [],
            on_collide))

    def OnDetaching(self, node_id='', sprite_id='', on_detach=[]):
        troll.execute(actions.OnDetaching(
            [self.id, node_id] if node_id else [self.id],
            [sprite_id] if sprite_id else [],
            on_detach))

    def PlayAnimation(self, script_id):
        troll.execute(actions.PlayAnimation(self.id, script_id))

    def StopAnimation(self, script_id=""):
        troll.execute(actions.StopAnimation(self.id, script_id))

    def PauseAnimation(self, script_id=""):
        troll.execute(actions.PauseAnimation(self.id, script_id))
