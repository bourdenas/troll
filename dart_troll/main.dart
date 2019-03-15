import 'lib/dart_troll.dart' as troll;
import 'lib/src/dk/intro.dart';

void main() {
  print('Hello, Troll!');

  troll.init("Dart Kong", "../samples/donkey_kong/data/");

  IntroScene().start();

  troll.run();

  print('bye, bye...');
}
