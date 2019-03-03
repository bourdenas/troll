import dk.controls
import dk.sprites
import proto.scene_pb2
import pytroll.audio
import pytroll.events
import pytroll.input
import pytroll.scene


class Level1Scene(pytroll.scene.PyTrollScene):
    def __init__(self):
        self.scene = proto.scene_pb2.Scene()
        self.scene.id = 'level1'
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

        for i in range(1, 7):
            platforms[i].Collapse()

        broken_ladder_positions = [
            (250, 446, -5), (250, 416, -5),
            (220, 326, -5), (220, 295, -5),
            (380, 266, -5), (380, 230, -5),
            (274, 198, -5), (274, 170, -5),
        ]
        [dk.sprites.Ladder(100 + i).Create(pos, frame_index=1)
         for i, pos in enumerate(broken_ladder_positions)]

        ladder_positions = [
            (210, 72, -5), (242, 72, -5),
            (210, 112, -5), (242, 112, -5),
            (338, 112, -5),

            (444, 420, -5), (280, 358, -5), (148, 353, -5),
            (310, 297, -5), (444, 294, -5),
            (148, 235, -5), (226, 235, -5),
            (444, 172, -5),
        ]
        [dk.sprites.Ladder(i).Create(pos, frame_index=0)
         for i, pos in enumerate(ladder_positions)]

        dk.sprites.Princess('princess').Create((270, 50, -1), frame_index=1)
        dk.sprites.Barrel('stack').Create((80, 88, 0), frame_index=5)
        dk.sprites.Barrel('base').Create((100, 428, 0), frame_index=6)

        donkey_kong = dk.sprites.DonkeyKong(
            'dk').Create((126, 90, 0), frame_index=0)
        donkey_kong.PlayAnimationById('dk_barrel_throw')

        pytroll.events.OnEvent(
            pytroll.events.AnimationScriptPartDone(
                donkey_kong.id, 'dk_barrel_throw', 'place_barrel'),
            lambda: dk.sprites.Barrel().Create(
                (215, 134, 0), frame_index=1).Roll(),
            permanent=True)

        dk.controls.mario = dk.sprites.Mario(
            'mario').Create((140, 428, 0), frame_index=15)

        pytroll.audio.PlayMusic('main_loop', repeat=0)

    def HandleInput(self, input_event):
        dk.controls.InputHandler(input_event)
