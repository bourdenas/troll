#ifndef TROLL_TROLL_TEST_TEST_UTIL_H_
#define TROLL_TROLL_TEST_TEST_UTIL_H_

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace troll {

// Convenience function for converting a text string into a proto Message of the
// template type.
template <class Message>
Message ParseProto(const std::string& text) {
  Message msg;
  EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(text, &msg))
      << "Failed to parse proto:\n"
      << text;
  return msg;
}

}  // namespace troll

// GMock matcher to be used for proto message equality. Example useage:
// EXPECT_THAT(actual_proto, EqualsProto(expected_proto));
//
// NB: The matcher is not type-safe. It is possible that the two protos are of
// different type.
MATCHER_P(EqualsProto, expected, expected.DebugString()) {
  std::string expected_serialized, actual_serialized;
  expected.SerializeToString(&expected_serialized);
  arg.SerializeToString(&actual_serialized);

  *result_listener << "\n" << arg.DebugString();
  return expected_serialized == actual_serialized;
}

#endif  // TROLL_TROLL_TEST_TEST_UTIL_H_
