import 'lib/dart_troll.dart' as troll;
import 'lib/intro.dart';

void main() {
  print('Hello, Troll!');

  troll.init("Dart Kong", "../samples/donkey_kong/data/");

  final intro = IntroScene();
  intro.transition();
  intro.setup();

  troll.run();

  print('bye, bye...');
}
