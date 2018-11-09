import troll


def OnEvent(event_id, handler, permanent=False):
    return troll.register_event_handler(event_id, handler, permanent)


def Cancel(event_id, handler_id):
    troll.cancel_event_handler(event_id, handler_id)


def AnimationScriptDone(node_id, script_id):
    return '.'.join((node_id, script_id, 'done'))


def AnimationScriptRepeat(node_id, script_id):
    return '.'.join((node_id, script_id, 'repeat'))


def AnimationScriptPartDone(node_id, script_id, animation_id):
    return '.'.join((node_id, script_id, animation_id, 'done'))
