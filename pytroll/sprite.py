import pytroll.actions
import pytroll.events
import troll


class Sprite:
    def __init__(self, id, sprite_id):
        self.id = id
        self.sprite_id = sprite_id

    def Create(self, position, frame_index=0):
        action = pytroll.actions.Create(
            self.id, self.sprite_id, frame_index, position)
        troll.execute(action.SerializeToString())
        return self

    def Destroy(self):
        action = pytroll.actions.Destroy(self.id)
        troll.execute(action.SerializeToString())

    def Position(self, position):
        action = pytroll.actions.Position(self.id, position)
        troll.execute(action.SerializeToString())

    def Move(self, vec):
        action = pytroll.actions.Move(self.id, vec)
        troll.execute(action.SerializeToString())

    def OnCollision(self, node_id='', sprite_id='', on_collide=[]):
        action = pytroll.actions.OnCollision(
            [self.id, node_id] if node_id else [self.id],
            [sprite_id] if sprite_id else [],
            on_collide)
        troll.execute(action.SerializeToString())

    def OnDetaching(self, node_id='', sprite_id='', on_detach=[]):
        action = pytroll.actions.OnDetaching(
            [self.id, node_id] if node_id else [self.id],
            [sprite_id] if sprite_id else [],
            on_detach)
        troll.execute(action.SerializeToString())

    def PlayAnimationScript(self, script, on_done=None):
        action = pytroll.actions.PlayAnimationScript(self.id, script)
        troll.execute(action.SerializeToString())
        if on_done:
            pytroll.events.OnEvent(
                pytroll.events.AnimationScriptDone(self.id, script.id),
                on_done)

    def PlayAnimationById(self, script_id, on_done=None):
        action = pytroll.actions.PlayAnimation(self.id, script_id)
        troll.execute(action.SerializeToString())
        if on_done:
            pytroll.events.OnEvent(
                pytroll.events.AnimationScriptDone(self.id, script_id),
                on_done)

    def StopAnimation(self, script_id=""):
        action = pytroll.actions.StopAnimation(self.id, script_id)
        troll.execute(action.SerializeToString())

    def PauseAnimation(self, script_id=""):
        action = pytroll.actions.PauseAnimation(self.id, script_id)
        troll.execute(action.SerializeToString())
