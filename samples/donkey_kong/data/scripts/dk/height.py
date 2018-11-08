import dk.scenes
import dk.sprites
import proto.animation_pb2
import pytroll.audio
import pytroll.events
import pytroll.scene


def init():
    donkey_kong = dk.sprites.DonkeyKong('dk')
    donkey_kong.Create((300, 382, 0), frame_index=10)
    pytroll.audio.PlayMusic('height')

    script = proto.animation_pb2.AnimationScript()
    script.id = 'height'
    script.animation.add().timer.delay = 3500
    donkey_kong.PlayAnimationScript(script)

    pytroll.events.OnEvent(
        pytroll.events.AnimationScriptDone(donkey_kong.id, 'height'),
        lambda: pytroll.scene.ChangeScene(dk.scenes.Level1Scene()))
