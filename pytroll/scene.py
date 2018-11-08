import pytroll.actions
import troll


def ChangeScene(scene_id):
    action = pytroll.actions.ChangeScene(scene_id)
    troll.execute(action.SerializeToString())
