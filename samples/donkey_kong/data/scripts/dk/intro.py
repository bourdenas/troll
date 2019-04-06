import dk.height
import dk.sprites
import proto.animation_pb2
import proto.key_binding_pb2
import pytroll.audio
import pytroll.events
import pytroll.input
import pytroll.scene


class IntroScene(pytroll.scene.PyTrollScene):
    def __init__(self):
        self.scene = proto.scene_pb2.Scene()
        self.scene.id = 'intro'
        self.scene.viewport.width = 640
        self.scene.viewport.height = 480

    def Setup(self):
        platform_sizes = [3, 13, 13, 13, 13, 13, 14]
        platforms = [dk.sprites.Platform(i, size)
                     for i, size in enumerate(platform_sizes)]
        platform_positions = [
            (262, 97, -3), (80, 155, -3), (112, 210, -3), (80, 275, -3),
            (112, 335, -3), (80, 395, -3), (80, 455, -3),
        ]
        for platform, pos in zip(platforms, platform_positions):
            platform.Create(pos)

        ladder_positions = [
            (210, 72, -1), (242, 72, -1),
            (210, 112, -1), (242, 112, -1),
            (338, 112, -1),
            (305, 170, -1), (338, 170, -1),
            (305, 210, -1), (338, 210, -1),
            (305, 250, -1), (338, 250, -1),
            (305, 290, -1), (338, 290, -1),
            (305, 330, -1), (338, 330, -1),
            (305, 370, -1), (338, 370, -1),
            (305, 410, -1), (338, 410, -1),
        ]
        ladders = [dk.sprites.Ladder(i).Create(pos, frame_index=0)
                   for i, pos in enumerate(ladder_positions)]

        donkey_kong = dk.sprites.DonkeyKong()
        donkey_kong.Create((300, 382, 0), frame_index=4)
        self.__Cutscene(donkey_kong, platforms, ladders)

    def __Cutscene(self, donkey_kong, platforms, ladders):
        script = proto.animation_pb2.AnimationScript()
        script.id = 'intro'
        script.animation.add().run_script.script_id = 'dk_climb'
        script.animation.add().timer.delay = 1000
        script.animation.add().run_script.script_id = 'dk_landing'
        script.animation.add().timer.delay = 300

        destroy_platforms = script.animation.add()
        destroy_platforms.termination = proto.animation_pb2.Animation.ALL
        goto = destroy_platforms.go_to
        goto.destination.x, goto.destination.y = 130, 90
        goto.step = 1
        goto.delay = 18
        destroy_platforms.run_script.script_id = 'dk_jump'

        script.animation.add().timer.delay = 300
        script.animation.add().run_script.script_id = 'dk_taunt'
        script.animation.add().timer.delay = 1000
        donkey_kong.PlayAnimationScript(
            script, lambda e: self.Transition(dk.height.HeightScene()))
        pytroll.audio.PlayMusic('intro')

        def LandingHandler():
            dk.sprites.Princess().Create((270, 50, -1), frame_index=1)
            platforms[1].Collapse()
            for i in range(4, 7):
                ladders[-(i * 2 + 1)].Destroy()
                ladders[-(i * 2 + 2)].Destroy()
        pytroll.events.OnEvent(
            pytroll.events.AnimationScriptDone(donkey_kong.id, 'dk_landing'),
            lambda e: LandingHandler())

        def DestroyPlatform(index):
            platforms[index].Collapse()
            if index < 6:
                donkey_kong.PlayAnimationById(
                    'dk_jump', lambda e: DestroyPlatform(index + 1))
        pytroll.events.OnEvent(
            pytroll.events.AnimationScriptDone(donkey_kong.id, 'dk_jump'),
            lambda e: DestroyPlatform(2))

        script = proto.animation_pb2.AnimationScript()
        script.animation.add().flash.repeat = 1
        for i in range(4):
            script.animation[0].flash.delay = 700 * (i + 1)
            ladders[-(i * 2 + 1)].PlayAnimationScript(script)
            ladders[-(i * 2 + 2)].PlayAnimationScript(script)

    def HandleInput(self, input_event):
        if input_event.HasField('key_event'):
            pressed = proto.key_binding_pb2.Trigger.PRESSED
            if input_event.key_event.key == 'SPACE' and \
                    input_event.key_event.key_state == pressed:
                self.Transition(dk.height.HeightScene())
