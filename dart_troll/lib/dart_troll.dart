library dart_troll;

import 'dart:typed_data';
import 'dart-ext:dart_troll';

void init(String name, String resourcePath) native "NativeInit";

void run() native "NativeRun";

void execute(Uint8List action) native "NativeExecute";

Uint8List eval(Uint8List query) native "NativeEval";

int registerEventHandler(String eventId, void Function(Uint8List) handler,
    {bool permanent = false}) native "NativeRegisterEventHandler";

int unregisterEventHandler(String eventId, int handlerId)
    native "NativeUnregisterEventHandler";

int registerInputHandler(void Function(Uint8List) handler)
    native "NativeRegisterInputHandler";

int unregisterInputHandler(int handlerId) native "NativeUnregisterInputHandler";
