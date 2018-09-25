import proto.action_pb2
import sprites
import troll


def init():
    objects = [
        sprites.Mario('mario'), sprites.Mario('mario_clone'),
        sprites.Mario('third_mario'),
    ]

    objects[0].Create(15, (100, 120))
    objects[0].PlayAnimation('mario_walk_right')
    objects[1].Create(0, (300, 60))
    objects[2].Create(0, (500, 120))
    objects[2].PlayAnimation('mario_patrol')

    dk = sprites.DonkeyKong('dk')
    dk.Create(0, (100, 200))
    dk.PlayAnimation('dk_taunt')

    platform_sizes = [3, 13, 13, 13, 13, 13, 14]
    platforms = [sprites.Platform('platform_' + str(i), size)
                 for i, size in enumerate(platform_sizes)]
    platform_positions = [
        (280, 97), (80, 155), (112, 210), (80, 275), (112, 335), (80, 395),
        (80, 455),
    ]

    for platform, position in zip(platforms, platform_positions):
        platform.Create(position)

    action = proto.action_pb2.Action()
    action.on_collision.scene_node_id.extend(['mario'])
    action.on_collision.sprite_id.extend(['mario'])
    action.on_collision.action.extend([dk.StopAnimationAction('dk_taunt')])
    action.on_collision.action.extend([dk.DestroyAction()])
    troll.execute(action.SerializeToString())
