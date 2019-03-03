import proto.scene_pb2
import pytroll.actions
import troll


class PyTrollScene:
    def __init__(self):
        self.scene = proto.scene_pb2.Scene()

    def Build(self):
        action = pytroll.actions.ChangeScene(self.scene)
        troll.execute(action.SerializeToString())

        self.handler_id = pytroll.input.RegisterHandler(
            lambda input_event: self.HandleInput(input_event))

    def Transition(self, scene):
        troll.transition_scene(scene)

    def Setup(self):
        pass

    def Cleanup(self):
        pytroll.input.CancelHandler(self.handler_id)

    def HandleInput(self, input_event):
        pass
