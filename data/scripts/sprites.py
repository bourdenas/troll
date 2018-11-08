import pytroll.sprite


class Mario(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'mario')


class Princess(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'princess')


class DonkeyKong(pytroll.sprite.Sprite):
    def __init__(self, id=''):
        pytroll.sprite.Sprite.__init__(self, id, 'dk')


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
        return self

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
    def __init__(self, id):
        pytroll.sprite.Sprite.__init__(self, 'ladder_' + str(id), 'ladder')


class Barrel(pytroll.sprite.Sprite):
    def __init__(self, id):
        pytroll.sprite.Sprite.__init__(self, 'barrel_' + str(id), 'barrel')
