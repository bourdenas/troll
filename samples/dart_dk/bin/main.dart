import 'package:dart_troll/dart_troll.dart' as troll;
import 'package:dart_dk/intro.dart';

void main() {
  print('Hello, Troll!');

  troll.init("Dart Kong", "../../samples/donkey_kong/data/");

  IntroScene().start();

  troll.run();

  print('bye, bye...');
}
