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

    def PlayAnimation(self, script_id):
        troll.execute(actions.PlayAnimation(self.id, script_id))

    def StopAnimation(self, script_id=""):
        troll.execute(actions.StopAnimation(self.id, script_id))

    def PauseAnimation(self, script_id=""):
        troll.execute(actions.PauseAnimation(self.id, script_id))
