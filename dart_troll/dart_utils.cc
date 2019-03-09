#include "dart_troll/dart_utils.h"

#include <absl/strings/str_cat.h>

namespace troll {

Dart_Handle HandleError(Dart_Handle handle) {
  if (Dart_IsError(handle)) Dart_PropagateError(handle);
  return handle;
}

void DartInternalError(Dart_NativeArguments args, std::string_view error) {
  Dart_SetReturnValue(args,
                      Dart_NewUnhandledExceptionError(UploadString(error)));
}

void DartArgsError(Dart_NativeArguments args, std::string_view func_name,
                   std::string_view arg_name) {
  DartInternalError(
      args, absl::StrCat("Invalid argument '", arg_name,
                         "' while calling function '", func_name, "'."));
}

Dart_Handle UploadString(std::string_view str) {
  if (str.empty()) return Dart_NewStringFromCString("");

  return Dart_NewStringFromUTF8(reinterpret_cast<const uint8_t*>(str.data()),
                                str.size());
}

std::string DownloadString(Dart_Handle dart_string) {
  const char* str;
  HandleError(Dart_StringToCString(dart_string, &str));
  return std::string(str);
}

Dart_Handle UploadBuffer(const uint8_t* buffer, int size) {
  Dart_Handle array = Dart_NewTypedData(Dart_TypedData_kUint8, size);
  if (size <= 0) return array;

  Dart_TypedData_Type td_type;
  void* td_data;
  intptr_t td_len;
  Dart_TypedDataAcquireData(array, &td_type, &td_data, &td_len);
  memmove(td_data, buffer, td_len);
  Dart_TypedDataReleaseData(array);

  return array;
}

}  // namespace troll
