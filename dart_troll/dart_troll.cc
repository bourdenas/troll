#include <string>

#include <dart_api.h>

#include "core/troll-core.h"
#include "dart_troll/dart_utils.h"

namespace troll {
Dart_NativeFunction ResolveName(Dart_Handle name, int argc,
                                bool* auto_setup_scope);
}  // namespace troll

// Dart native extension entrypoint.
DART_EXPORT Dart_Handle dart_troll_Init(Dart_Handle parent_library) {
  if (Dart_IsError(parent_library)) return parent_library;

  Dart_Handle result_code =
      Dart_SetNativeResolver(parent_library, troll::ResolveName, nullptr);
  if (Dart_IsError(result_code)) return result_code;

  return Dart_Null();
}

namespace troll {

TrollCore* core;

// Sets up troll-core.
void NativeInit(Dart_NativeArguments arguments) {
  const Dart_Handle name = HandleError(Dart_GetNativeArgument(arguments, 0));
  const Dart_Handle resource_path =
      HandleError(Dart_GetNativeArgument(arguments, 1));

  core = new TrollCore;
  core->Init(DownloadString(name), DownloadString(resource_path), nullptr);
}

// Start game engine execution.
void NativeRun(Dart_NativeArguments arguments) { core->Run(); }

// Execute an action through troll's executors system.
void NativeExecute(Dart_NativeArguments arguments) {
  const Dart_Handle action_buffer =
      HandleError(Dart_GetNativeArgument(arguments, 0));
  const auto action = DownloadProtoValue<Action>(action_buffer);
  core->action_manager()->Execute(action);
}

// Register an event handler.
void NativeRegisterEventHandler(Dart_NativeArguments arguments) {
  const Dart_Handle event_id =
      HandleError(Dart_GetNativeArgument(arguments, 0));
  const Dart_Handle handler = HandleError(Dart_GetNativeArgument(arguments, 1));
  const Dart_Handle permanent =
      HandleError(Dart_GetNativeArgument(arguments, 2));

  if (!Dart_IsClosure(handler)) {
    DartArgsError(arguments, "registerEventHandler", "handler");
    return;
  }

  int handler_id;
  if (DownloadBoolean(permanent)) {
    handler_id = core->event_dispatcher()->RegisterPermanent(
        DownloadString(event_id),
        [handler]() { HandleError(Dart_InvokeClosure(handler, 0, nullptr)); });
  } else {
    handler_id = core->event_dispatcher()->Register(
        DownloadString(event_id),
        [handler]() { HandleError(Dart_InvokeClosure(handler, 0, nullptr)); });
  }

  Dart_Handle result = HandleError(Dart_NewInteger(handler_id));
  Dart_SetReturnValue(arguments, result);
}

// Unregister an event handler.
void NativeUnregisterEventHandler(Dart_NativeArguments arguments) {
  const Dart_Handle event_id =
      HandleError(Dart_GetNativeArgument(arguments, 0));
  const Dart_Handle handler_id =
      HandleError(Dart_GetNativeArgument(arguments, 1));

  core->event_dispatcher()->Unregister(DownloadString(event_id),
                                       DownloadInt(handler_id));
}

// Resolves Darts calls to native functions by name.
Dart_NativeFunction ResolveName(Dart_Handle name, int argc,
                                bool* auto_setup_scope) {
  if (!Dart_IsString(name)) return nullptr;

  const auto func_name = DownloadString(name);
  if (func_name == "NativeInit") {
    return NativeInit;
  }
  if (func_name == "NativeRun") {
    return NativeRun;
  }
  if (func_name == "NativeExecute") {
    return NativeExecute;
  }
  if (func_name == "NativeRegisterEventHandler") {
    return NativeRegisterEventHandler;
  }
  if (func_name == "NativeUnregisterEventHandler") {
    return NativeUnregisterEventHandler;
  }

  return nullptr;
}

}  // namespace troll
