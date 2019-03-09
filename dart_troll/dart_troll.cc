#include <string>

#include <dart_api.h>

#include "core/troll-core.h"

Dart_NativeFunction ResolveName(Dart_Handle name, int argc,
                                bool* auto_setup_scope);

DART_EXPORT Dart_Handle dart_troll_Init(Dart_Handle parent_library) {
  if (Dart_IsError(parent_library)) return parent_library;

  Dart_Handle result_code =
      Dart_SetNativeResolver(parent_library, ResolveName, nullptr);
  if (Dart_IsError(result_code)) return result_code;

  return Dart_Null();
}

Dart_Handle HandleError(Dart_Handle handle) {
  if (Dart_IsError(handle)) Dart_PropagateError(handle);
  return handle;
}

troll::TrollCore* core;

void NativeInit(Dart_NativeArguments arguments) {
  const Dart_Handle name = HandleError(Dart_GetNativeArgument(arguments, 0));
  const char* cstr_name;
  HandleError(Dart_StringToCString(name, &cstr_name));

  const Dart_Handle resource_path =
      HandleError(Dart_GetNativeArgument(arguments, 1));
  const char* cstr_resource_path;
  HandleError(Dart_StringToCString(resource_path, &cstr_resource_path));

  core = new troll::TrollCore;
  core->Init(cstr_name, cstr_resource_path, nullptr);
}

void NativeRun(Dart_NativeArguments arguments) { core->Run(); }

Dart_NativeFunction ResolveName(Dart_Handle name, int argc,
                                bool* auto_setup_scope) {
  if (!Dart_IsString(name)) return nullptr;

  const char* cname;
  HandleError(Dart_StringToCString(name, &cname));
  const std::string_view func_name = cname;

  if (func_name == "NativeInit") {
    return NativeInit;
  }
  if (func_name == "NativeRun") {
    return NativeRun;
  }

  return nullptr;
}
