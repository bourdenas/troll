import pytroll.sprite
import proto.animation_pb2
import troll


class Mario(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'mario')


class DonkeyKong(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'dk')

    def IntroCutscene(self):
        script = proto.animation_pb2.AnimationScript()

        script.animation.add().run_script.script_id = 'dk_climb'
        script.animation.add().timer.delay = 1000
        script.animation.add().run_script.script_id = 'dk_landing'
        script.animation.add().timer.delay = 300

        destroy_platforms = script.animation.add()
        destroy_platforms.termination = proto.animation_pb2.Animation.ALL
        destroy_platforms.go_to.destination.x = 130
        destroy_platforms.go_to.destination.y = 90
        destroy_platforms.go_to.step = 1
        destroy_platforms.go_to.delay = 18
        destroy_platforms.run_script.script_id = 'dk_jump'

        def destroy_platform(dk, index):
            if index == 4:
                return
            dk.PlayAnimation(
                'dk_jump', lambda: destroy_platform(dk, index + 1))

        troll.on_event('.'.join((self.id, 'dk_jump', 'done')),
                       lambda: destroy_platform(self, 0))

        script.animation.add().timer.delay = 300
        script.animation.add().run_script.script_id = 'dk_taunt'

        return script


class PlatformPiece(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'platform')


class Platform(object):
    def __init__(self, id, size):
        self.pieces = [PlatformPiece(id + '_' + str(i)) for i in range(size)]

    def Create(self, position):
        platform_width = 32
        for i, piece in enumerate(self.pieces):
            piece.Create(
                (position[0] + i * platform_width, position[1], position[2]))


class Ladder(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'ladder')
