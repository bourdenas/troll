import troll
import proto.input_event_pb2


def __Unmarshal(encoded_input_event):
    e = proto.input_event_pb2.InputEvent()
    e.ParseFromString(encoded_input_event)
    return e


def __handlerUnmarshaling(handler):
    return lambda encoded_input_event: handler(
        __Unmarshal(encoded_input_event))


def RegisterHandler(handler):
    return troll.register_input_handler(__handlerUnmarshaling(handler))


def CancelHandler(handler_id):
    troll.cancel_input_handler(handler_id)
