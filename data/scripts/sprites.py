import pytroll.sprite
import proto.animation_pb2


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

        thrusting = script.animation.add()
        thrusting.frame_list.frame.extend([5])
        thrusting.frame_list.vertical_align = proto.animation_pb2.BOTTOM
        thrusting.go_to.destination.x = 300
        thrusting.go_to.destination.y = 70
        thrusting.go_to.step = 2
        thrusting.go_to.delay = 20

        landing = script.animation.add()
        landing.frame_list.frame.extend([0])
        landing.frame_list.vertical_align = proto.animation_pb2.BOTTOM
        landing.go_to.destination.x = 300
        landing.go_to.destination.y = 90
        landing.go_to.step = 2
        landing.go_to.delay = 20

        script.animation.add().timer.delay = 300

        destroy_platforms = script.animation.add()
        destroy_platforms.go_to.destination.x = 130
        destroy_platforms.go_to.destination.y = 90
        destroy_platforms.go_to.step = 1
        destroy_platforms.go_to.delay = 17
        destroy_platforms.run_script.script_id = 'dk_jump'

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
