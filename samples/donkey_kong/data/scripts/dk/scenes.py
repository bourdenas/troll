import proto.scene_pb2


def HeightScene():
    scene = proto.scene_pb2.Scene()
    script = scene.on_init.add()
    script.module = 'dk.height'
    script.function = 'init'
    return scene


def Level1Scene():
    scene = proto.scene_pb2.Scene()
    script = scene.on_init.add()
    script.module = 'dk.level1'
    script.function = 'init'
    return scene
