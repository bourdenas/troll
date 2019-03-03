import dk.level1
import dk.sprites
import proto.animation_pb2
import pytroll.audio
import pytroll.scene


class HeightScene(pytroll.scene.PyTrollScene):
    def __init__(self):
        self.scene = proto.scene_pb2.Scene()
        self.scene.id = 'height'
        self.scene.viewport.width = 640
        self.scene.viewport.height = 480

    def Setup(self):
        donkey_kong = dk.sprites.DonkeyKong('dk')
        donkey_kong.Create((300, 382, 0), frame_index=10)
        pytroll.audio.PlayMusic('height')

        script = proto.animation_pb2.AnimationScript()
        script.id = 'height'
        script.animation.add().timer.delay = 3500
        donkey_kong.PlayAnimationScript(
            script, lambda: self.Transition(dk.level1.Level1Scene()))
