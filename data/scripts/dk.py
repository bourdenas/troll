import actions
import sprites
import troll


def intro():
    platform_sizes = [3, 13, 13, 13, 13, 13, 14]
    platforms = [sprites.Platform('platform_' + str(i), size)
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
    for pos in ladder_positions:
        sprites.Ladder().Create(pos, frame_index=0)

    dk = sprites.DonkeyKong('dk')
    dk.Create((300, 382, 0), frame_index=4)
    dk.PlayAnimation('dk_climb')


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


# collision = actions.OnCollision(['mario'], ['mario'], [
#     actions.Create(dk.id, dk.sprite_id, 0, (100, 200, 0), packed=False)])
# troll.execute(collision)
