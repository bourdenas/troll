library dart_troll;

import 'dart:typed_data';
import 'dart-ext:dart_troll';

void init(String name, String resourcePath) native "NativeInit";

void run() native "NativeRun";

void execute(Uint8List action) native "NativeExecute";

Uint8List eval(Uint8List query) native "NativeEval";

int registerEventHandler(String event_id, Function handler,
    {bool permanent = false}) native "NativeRegisterEventHandler";

int unregisterEventHandler(String event_id, int handler_id)
    native "NativeUnregisterEventHandler";

int registerInputHandler(Function handler) native "NativeRegisterInputHandler";

int unregisterInputHandler(int handler_id)
    native "NativeUnregisterInputHandler";
