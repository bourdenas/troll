import troll_element


class Mario(troll_element.Sprite):
    def __init__(self, id=''):
        troll_element.Sprite.__init__(self, id, 'mario')


class DonkeyKong(troll_element.Sprite):
    def __init__(self, id=''):
        troll_element.Sprite.__init__(self, id, 'dk')


class PlatformPiece(troll_element.Sprite):
    def __init__(self, id=''):
        troll_element.Sprite.__init__(self, id, 'platform')


class Platform(object):
    def __init__(self, id, size):
        self.pieces = [PlatformPiece(id + '_' + str(i)) for i in range(size)]

    def Create(self, position):
        platform_width = 32
        for i, piece in enumerate(self.pieces):
            piece.Create(
                (position[0] + i * platform_width, position[1], position[2]))


class Ladder(troll_element.Sprite):
    def __init__(self, id=''):
        troll_element.Sprite.__init__(self, id, 'ladder')
