library dart_troll;

import 'dart:typed_data';
import 'dart-ext:dart_troll';

void init(String name, String resourcePath) native "NativeInit";

void run() native "NativeRun";

void execute(Uint8List action) native "NativeExecute";
