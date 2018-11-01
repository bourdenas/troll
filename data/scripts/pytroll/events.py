import troll


def OnEvent(event_id, handler):
    troll.on_event(event_id, handler)


def AnimationDone(node_id, script_id):
    return '.'.join((node_id, script_id, 'done'))
