library dart_troll;

import 'dart-ext:dart_troll';

void init(String name, String resourcePath) native "NativeInit";

void run(Function f) native "NativeRun";
