import proto.animation_pb2
import proto.scene_pb2
import pytroll.audio
import pytroll.events
import pytroll.scene
import sprites


def cutscene(dk, platforms, ladders):
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
    dk.PlayAnimationScript(script)
    pytroll.audio.PlayMusic('intro')

    def LandingHandler():
        sprites.Princess().Create((270, 50, -1), frame_index=1)
        platforms[1].Collapse()
        for i in range(4, 7):
            ladders[-(i * 2 + 1)].Destroy()
            ladders[-(i * 2 + 2)].Destroy()
    pytroll.events.OnEvent(
        pytroll.events.AnimationScriptDone(dk.id, 'dk_landing'),
        lambda: LandingHandler())

    def DestroyPlatform(index):
        platforms[index].Collapse()
        if index < 6:
            dk.PlayAnimation(
                'dk_jump', lambda: DestroyPlatform(index + 1))
    pytroll.events.OnEvent(
        pytroll.events.AnimationScriptDone(dk.id, 'dk_jump'),
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

    def HeightScene():
        scene = proto.scene_pb2.Scene()
        script = scene.on_init.add()
        script.module = 'dk'
        script.function = 'height'
        return scene

    pytroll.events.OnEvent(
        pytroll.events.AnimationScriptDone(dk.id, 'intro'),
        lambda: pytroll.scene.ChangeScene(HeightScene()))


def height():
    dk = sprites.DonkeyKong('dk')
    dk.Create((300, 382, 0), frame_index=10)
    pytroll.audio.PlayMusic('height')

    script = proto.animation_pb2.AnimationScript()
    script.id = 'height'
    script.animation.add().timer.delay = 3500
    dk.PlayAnimationScript(script)

    def Level1Scene():
        scene = proto.scene_pb2.Scene()
        script = scene.on_init.add()
        script.module = 'dk'
        script.function = 'level1'
        return scene

    pytroll.events.OnEvent(
        pytroll.events.AnimationScriptDone(dk.id, 'height'),
        lambda: pytroll.scene.ChangeScene(Level1Scene()))


def level1():
    platform_sizes = [3, 13, 13, 13, 13, 13, 14]
    platforms = [sprites.Platform(i, size)
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
    [sprites.Ladder(100 + i).Create(pos, frame_index=1)
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
    [sprites.Ladder(i).Create(pos, frame_index=0)
     for i, pos in enumerate(ladder_positions)]

    sprites.Princess('princess').Create((270, 50, -1), frame_index=1)
    sprites.Barrel('stack').Create((80, 88, 0), frame_index=5)
    sprites.Barrel('base').Create((100, 428, 0), frame_index=6)

    dk = sprites.DonkeyKong('dk').Create((126, 90, 0), frame_index=0)
    dk.PlayAnimation('dk_barrel_throw')

    pytroll.events.OnEvent(
        pytroll.events.AnimationScriptPartDone(
            dk.id, 'dk_barrel_throw', 'place_barrel'),
        lambda: sprites.Barrel().Create((215, 134, 0), frame_index=1).Roll(),
        permanent=True)

    sprites.Mario('mario').Create((140, 428, 0), frame_index=15)
    pytroll.audio.PlayMusic('main_loop', repeat=0)
