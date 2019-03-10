import 'package:dart_troll/dart_troll.dart' as troll;
import 'package:dart_troll/src/proto/action.pb.dart';
import 'package:dart_troll/src/proto/scene.pb.dart' as troll_proto;

class Scene {
  troll_proto.Scene scene;

  void transition() {
    final action = Action()..changeScene = (ChangeSceneAction()..scene = scene);
    troll.execute(action.writeToBuffer());
  }
}
