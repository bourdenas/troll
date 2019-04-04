#include "core/scene-node-pattern.h"

#include <regex>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

namespace troll {

bool SceneNodePattern::Parse(const std::string& pattern_str) {
  static const std::regex re(R"(\$(this)?\.\{(.*)\})", std::regex::optimize);

  std::smatch match;
  if (!std::regex_match(pattern_str, match, re)) {
    mode = RetrievalMode::INVALID;
    return false;
  }

  LOG_IF(ERROR, match.size() != 3)
      << "Invalid node search pattern: '" << pattern_str << "'";

  if (match[1].length() == 0) {
    mode = RetrievalMode::GLOBAL;
  } else if (match[1] == "this") {
    mode = RetrievalMode::LOCAL;
  } else {
    mode = RetrievalMode::INVALID;
  }

  if (!google::protobuf::TextFormat::ParseFromString(match[2], &pattern)) {
    mode = RetrievalMode::INVALID;
    return false;
  }
  return true;
}

}  // namespace troll
