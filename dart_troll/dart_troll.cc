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

  return nullptr;
}

}  // namespace troll
