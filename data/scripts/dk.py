import proto.animation_pb2
import pytroll.audio
import pytroll.events
import sprites


def cutscene(dk, platforms, ladders):
    script = proto.animation_pb2.AnimationScript()
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
    dk.PlayAnimationScript(script)
    pytroll.audio.PlayMusic('intro')

    def LandingHandler():
        princess = sprites.Princess()
        princess.Create((270, 50, -1), frame_index=1)
        platforms[1].Collapse()
        for i in range(4, 7):
            ladders[-(i * 2 + 1)].Destroy()
            ladders[-(i * 2 + 2)].Destroy()
    pytroll.events.OnEvent(
        pytroll.events.AnimationDone(dk.id, 'dk_landing'),
        lambda: LandingHandler())

    def DestroyPlatform(index):
        platforms[index].Collapse()
        if index < 6:
            dk.PlayAnimation(
                'dk_jump', lambda: DestroyPlatform(index + 1))
    pytroll.events.OnEvent(
        pytroll.events.AnimationDone(dk.id, 'dk_jump'),
        lambda: DestroyPlatform(2))

    script = proto.animation_pb2.AnimationScript()
    script.animation.add().flash.repeat = 1
    for i in range(4):
        script.animation[0].flash.delay = 700 * (i + 1)
        ladders[-(i * 2 + 1)].PlayAnimationScript(script)
        ladders[-(i * 2 + 2)].PlayAnimationScript(script)


def intro():
    platform_sizes = [3, 13, 13, 13, 13, 13, 14]
    platforms = [sprites.Platform(i, size)
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
    ladders = [sprites.Ladder(i).Create(pos, frame_index=0)
               for i, pos in enumerate(ladder_positions)]

    dk = sprites.DonkeyKong('dk')
    dk.Create((300, 382, 0), frame_index=4)
    cutscene(dk, platforms, ladders)


def level1():
    ladder_positions = [
        (230, 70, -1), (262, 70, -1), (338, 112, -1), (250, 442, -1),
        (250, 416, -1), (444, 417, -1), (280, 360, -1), (148, 356, -1),
        (220, 324, -1), (220, 295, -1), (310, 300, -1), (444, 296, -1),
        (380, 264, -1), (380, 230, -1), (220, 236, -1), (148, 236, -1),
        (274, 196, -1), (274, 170, -1), (444, 172, -1),
    ]
    for pos in ladder_positions:
        sprites.Ladder().Create(pos, frame_index=0)
