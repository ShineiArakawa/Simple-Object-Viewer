#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace simview {
namespace util {

class StringUtil {
 public:
  static std::vector<std::string> splitText(const std::string& s,
                                            const char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
      if (!item.empty() && item != "null") {
        tokens.push_back(item);
      }
    }

    return tokens;
  }

  static std::string replace(const std::string& s,
                             const char& from,
                             const char& to) {
    std::string replaced = "";

    for (const char& charactor : s) {
      if (charactor == from) {
        replaced += to;
      } else {
        replaced += charactor;
      }
    }

    return replaced;
  }

  static std::string remove(const std::string& s,
                            const char& target) {
    std::string replaced = "";

    for (const char& charactor : s) {
      if (charactor != target) {
        replaced += charactor;
      }
    }

    return replaced;
  }
};

}  // namespace util
}  // namespace simview