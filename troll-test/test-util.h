#ifndef TROLL_TROLL_TEST_TEST_UTIL_H_
#define TROLL_TROLL_TEST_TEST_UTIL_H_

#include <ostream>
#include <sstream>

#include <google/protobuf/text_format.h>

namespace troll {

// NOTE: Cannot use StringMaker because of the polymorphic type. Looks ugly, but
// the operator needs to be overloaded before including <catch.hpp>.
std::ostream& operator<<(std::ostream& os,
                         const google::protobuf::Message& msg) {
  return os << msg.DebugString();
}

}  // namespace troll

#include <catch.hpp>

namespace troll {

// Convenience function for converting a text string into a proto Message of the
// template type.
template <typename Message>
Message ParseProto(const std::string& text) {
  Message msg;
  if (!google::protobuf::TextFormat::ParseFromString(text, &msg)) {
    FAIL("Failed to parse proto:\n" << text);
  }
  return msg;
}

template <typename Message>
class ProtoMatcher : public Catch::MatcherBase<Message> {
 public:
  ProtoMatcher(const Message& expected) : expected_(expected) {}

  bool match(const Message& actual) const override {
    return expected_.SerializeAsString() == actual.SerializeAsString();
  }

  std::string describe() const override {
    std::ostringstream ss;
    ss << "\ndoes not match:\n" << expected_.DebugString();
    return ss.str();
  }

 private:
  Message expected_;
};

template <typename Message>
ProtoMatcher<Message> EqualsProto(const Message& expected) {
  return ProtoMatcher<Message>(expected);
}

}  // namespace troll

#endif  // TROLL_TROLL_TEST_TEST_UTIL_H_
