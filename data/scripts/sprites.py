import troll_element


class Mario(troll_element.TrollElement):
  def __init__(self, id):
      troll_element.TrollElement.__init__(self, id, 'mario')


class DonkeyKong(troll_element.TrollElement):
  def __init__(self, id):
      troll_element.TrollElement.__init__(self, id, 'dk')


class PlatformPiece(troll_element.TrollElement):
  def __init__(self, id):
      troll_element.TrollElement.__init__(self, id, 'platform')


class Platform(object):
  def __init__(self, id, size):
    self.pieces = [PlatformPiece(id + '_' + str(i)) for i in range(size)]

  def Create(self, position):
    platform_width = 32
    for i, piece in enumerate(self.pieces):
      piece.Create(0, (position[0] + i * platform_width, position[1]))
