#include "core/scene-node-query.h"

#include <regex>

#include <glog/logging.h>
#include <google/protobuf/text_format.h>

namespace troll {

bool SceneNodeQuery::Parse(const std::string& pattern) {
  static const std::regex re(
      R"(\$(ctx)?\.\{(.*)\})", std::regex::optimize);

  std::smatch match;
  if (!std::regex_match(pattern, match, re)) {
    mode = SceneNodeQuery::INVALID;
    return false;
  }

  LOG_IF(ERROR, match.size() != 3) << "Invalid node search pattern: '"
                                   << pattern << "'";

  if (match[1].empty()) {
    mode = SceneNodeQuery::GLOBAL;
  } else if (match[2] == "ctx") {
    mode = SceneNodeQuery::LOCAL;
  } else {
    mode = SceneNodeQuery::INVALID;
  }

  if (!google::protobuf::TextFormat::ParseFromString(match[2], &pattern)) {
    mode = SceneNodeQuery::INVALID;
    return false;
  }
  return true;
}

}  // namespace troll
