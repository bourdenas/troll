#ifndef TROLL_DART_TROLL_DART_UTILS_H_
#define TROLL_DART_TROLL_DART_UTILS_H_

#include <string>

#include <dart_api.h>

namespace troll {

Dart_Handle HandleError(Dart_Handle handle);

void DartInternalError(Dart_NativeArguments args, std::string_view error);
void DartArgsError(Dart_NativeArguments args, std::string_view func_name,
                   std::string_view arg_name);

Dart_Handle UploadString(std::string_view string);
std::string DownloadString(Dart_Handle dart_string);

Dart_Handle UploadBuffer(const uint8_t* buffer, int size);

template <typename InputProto>
Dart_Handle UploadProtoValue(const InputProto& input) {
  std::string buffer = input.SerialiseAsString();
  return UploadDartBuffer(buffer.data(), buffer.size());
}

template <typename OutputProto>
OutputProto DownloadProtoValue(Dart_Handle dart_buffer) {
  OutputProto output;

  intptr_t len;
  HandleError(Dart_ListLength(dart_buffer, &len));

  if (len <= 0) return output;

  std::vector<uint8_t> buffer;
  buffer.resize(len);
  HandleError(Dart_ListGetAsBytes(dart_buffer, 0, buffer.data(), len));

  output.ParseFromArray(buffer.data(), static_cast<int>(buffer.size()));
  return output;
}

}  // namespace troll

#endif  // TROLL_DART_TROLL_DART_UTILS_H_
