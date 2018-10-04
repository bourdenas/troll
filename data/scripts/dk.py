import actions
import sprites
import troll


def foo():
    mario2.PlayAnimation('mario_patrol')
    print('foo')


def init():
    platform_sizes = [3, 13, 13, 13, 13, 13, 14]
    platforms = [sprites.Platform('platform_' + str(i), size)
                 for i, size in enumerate(platform_sizes)]
    platform_positions = [
        (280, 97, -1), (80, 155, -1), (112, 210, -1), (80, 275, -1),
        (112, 335, -1), (80, 395, -1), (80, 455, -1),
    ]

    for platform, position in zip(platforms, platform_positions):
        platform.Create(position)

    mario = sprites.Mario('mario')
    mario.Create(15, (100, 120, 1))
    # mario.PlayAnimation('mario_walk_right')

    mario3 = sprites.Mario('mario3')
    mario3.Create(0, (500, 120, 1))
    mario3.PlayAnimation('mario_patrol')

    dk = sprites.DonkeyKong('dk')
    dk.Create(0, (100, 200, 0))
    dk.PlayAnimation('dk_taunt')

    mario2 = sprites.Mario('mario2')
    mario2.Create(0, (85, 220, 1))

    collision = actions.OnCollision(['mario'], ['mario'], [
        actions.Create(dk.id, dk.sprite_id, 0, (100, 200, 0), packed=False)])
    # troll.execute(collision)

    collision = actions.OnCollision(['mario'], ['mario'], [
        actions.StopAnimation(dk.id, 'dk_taunt', packed=False),
        actions.Destroy(dk.id, packed=False),
    ])
    troll.execute(collision)

    collision = actions.OnCollision(['mario'], ['mario'], [
        actions.Call('dk', 'foo', packed=False),
    ])
    # troll.execute(collision)
