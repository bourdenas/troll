library dart_troll;

import 'dart:typed_data';
import 'dart-ext:dart_troll';

/// Setups the game engine.
void init(String name, String resourcePath) native "NativeInit";

/// Starts the main event loop of the game engine.
void run() native "NativeRun";

/// Executes a troll.Action in the game engine.
void execute(Uint8List action) native "NativeExecute";

/// Evals a troll.Query and retuns the result.
Uint8List eval(Uint8List query) native "NativeEval";

/// Registers a [handler] that is called when [eventId] triggers.
///
/// The [handler] receives a troll.Event as argument.
/// If [permanent] is true the [handler] will be called every time the event
/// triggers, otherwise it will be called only once the first time [eventId]
/// triggers.
/// Returns a unique ID for the installed event handler.
int registerEventHandler(String eventId, void Function(Uint8List) handler,
    {bool permanent = false}) native "NativeRegisterEventHandler";

/// Removes the event handler registered for [eventId] that corresponds to
/// [handlerId].
///
/// This is a no-op if the event handler was not permanent and it already
/// triggered in response to [eventId].
void unregisterEventHandler(String eventId, int handlerId)
    native "NativeUnregisterEventHandler";

/// Registers a [handler] that is called when [eventId] triggers.
///
/// The [handler] receives a troll.InputEvent as argument.
/// Returns a unique ID for the installed input handler.
int registerInputHandler(void Function(Uint8List) handler)
    native "NativeRegisterInputHandler";

/// Removes the input handler corresponding to [handlerId].
void unregisterInputHandler(int handlerId)
    native "NativeUnregisterInputHandler";
