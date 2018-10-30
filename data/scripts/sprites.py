import pytroll.sprite


class Mario(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'mario')


class DonkeyKong(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'dk')


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
