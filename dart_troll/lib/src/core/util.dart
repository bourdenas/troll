import 'package:dart_troll/src/proto/primitives.pb.dart';

/// Returns a Vector from input list.
///
/// Throws [ArgumentError] if the list does not have proper size (2 or 3).
/// Treats the third dimension as optional.
Vector makeVector(List<int> list) {
  if (list.length < 2 || list.length > 3) {
    throw ArgumentError('list size for position have length 2 or 3');
  }

  var vec = Vector()
    ..x = list[0].toDouble()
    ..y = list[1].toDouble();

  if (list.length == 3) vec.z = list[2].toDouble();
  return vec;
}
