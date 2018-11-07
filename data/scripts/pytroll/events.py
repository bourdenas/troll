import troll


def OnEvent(event_id, handler):
    troll.on_event(event_id, handler)


def AnimationScriptDone(node_id, script_id):
    return '.'.join((node_id, script_id, 'done'))


def AnimationScriptRepeat(node_id, script_id):
    return '.'.join((node_id, script_id, 'repeat'))


def AnimationScriptPartDone(node_id, script_id, animation_id):
    return '.'.join((node_id, script_id, animation_id, 'done'))
