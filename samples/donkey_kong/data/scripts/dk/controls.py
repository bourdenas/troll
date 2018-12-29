import proto.key_binding_pb2

mario = None

__key_mapping = {
    ('LEFT', proto.key_binding_pb2.Trigger.PRESSED): [
        lambda: mario.PlayAnimation('mario_move_left'),
    ],
    ('LEFT', proto.key_binding_pb2.Trigger.RELEASED): [
        lambda: mario.StopAnimation('mario_move_left'),
    ],
    ('RIGHT', proto.key_binding_pb2.Trigger.PRESSED): [
        lambda: mario.PlayAnimation('mario_move_right'),
    ],
    ('RIGHT', proto.key_binding_pb2.Trigger.RELEASED): [
        lambda: mario.StopAnimation('mario_move_right'),
    ],
    ('UP', proto.key_binding_pb2.Trigger.PRESSED): [
        lambda: mario.PlayAnimation('mario_climb_up'),
    ],
    ('UP', proto.key_binding_pb2.Trigger.RELEASED): [
        lambda: mario.StopAnimation('mario_climb_up'),
    ],
    ('DOWN', proto.key_binding_pb2.Trigger.PRESSED): [
        lambda: mario.PlayAnimation('mario_climb_down'),
    ],
    ('DOWN', proto.key_binding_pb2.Trigger.RELEASED): [
        lambda: mario.StopAnimation('mario_climb_down'),
    ],
    ('SPACE', proto.key_binding_pb2.Trigger.PRESSED): [
        lambda: mario.PlayAnimation('mario_jump'),
    ],
}


def InputHandler(input_event):
    if input_event.HasField('key_event'):
        key = (input_event.key_event.key, input_event.key_event.key_state)
        handlers = []
        try:
            handlers = __key_mapping[key]
        except KeyError:
            pass

        for handler in handlers:
            handler()
