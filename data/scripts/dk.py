import actions
import sprites
import troll

mario2 = sprites.Mario('mario2')
mario2.Create(0, (300, 60))


def foo():
    mario2.PlayAnimation('mario_patrol')
    print('foo')


def init():
    platform_sizes = [3, 13, 13, 13, 13, 13, 14]
    platforms = [sprites.Platform('platform_' + str(i), size)
                 for i, size in enumerate(platform_sizes)]
    platform_positions = [
        (280, 97), (80, 155), (112, 210), (80, 275), (112, 335), (80, 395),
        (80, 455),
    ]

    for platform, position in zip(platforms, platform_positions):
        platform.Create(position)

    mario = sprites.Mario('mario')
    mario.Create(15, (100, 120))
    mario.PlayAnimation('mario_walk_right')

    mario3 = sprites.Mario('third_mario')
    mario3.Create(0, (500, 120))
    mario3.PlayAnimation('mario_patrol')

    dk = sprites.DonkeyKong('dk')
    dk.Create(0, (100, 200))
    dk.PlayAnimation('dk_taunt')

    collision = actions.OnCollision(['mario'], ['mario'], [
        actions.StopAnimation(dk.id, 'dk_taunt', packed=False),
        actions.Destroy(dk.id, packed=False),
    ])
    troll.execute(collision)

    collision = actions.OnCollision(['mario'], ['mario'], [
        actions.Call(foo, packed=False),
    ])
    troll.execute(collision)
