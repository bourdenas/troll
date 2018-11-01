import pytroll.sprite
import proto.animation_pb2
import troll


class Mario(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'mario')


class DonkeyKong(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'dk')

    def IntroCutscene(self, platforms):
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

        def DestroyTopPlatform():
            platforms[1].Collapse()
        troll.on_event('.'.join((self.id, 'dk_landing', 'done')),
                       lambda: DestroyTopPlatform())

        def DestroyPlatform(dk, index):
            platforms[index + 2].Collapse()
            if index == 4:
                return
            dk.PlayAnimation(
                'dk_jump', lambda: DestroyPlatform(dk, index + 1))
        troll.on_event('.'.join((self.id, 'dk_jump', 'done')),
                       lambda: DestroyPlatform(self, 0))

        script.animation.add().timer.delay = 300
        script.animation.add().run_script.script_id = 'dk_taunt'
        return script


class PlatformPiece(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'platform')


class Platform(object):
    def __init__(self, id, size):
        self.id = id
        self.pieces = [PlatformPiece(
            'platform_' + str(id) + '_' + str(i)) for i in range(size)]

    def Create(self, position):
        platform_width = 32
        for i, piece in enumerate(self.pieces):
            piece.Create(
                (position[0] + i * platform_width, position[1], position[2]))

    def Collapse(self):
        if self.id == 1:
            for i in range(0, 4):
                self.pieces[-(i + 1)].Move((0, 4 - i, 0))
        elif self.id == 6:
            for i in range(0, 7):
                self.pieces[-(i + 1)].Move((0, i, 0))
                self.pieces[i].Move((0, 7, 0))
        elif self.id % 2 == 0:
            for i, piece in enumerate(self.pieces):
                piece.Move((0, len(self.pieces) - (i+1), 0))
        elif self.id % 2 == 1:
            for i, piece in enumerate(self.pieces):
                piece.Move((0, i, 0))


class Ladder(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'ladder')
