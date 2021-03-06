#pragma once

#include <string>

namespace imog {

class Files {
public:
  static bool ok(const std::string& path, bool log = false);
  static bool pathExists(const std::string& path);
  static std::string pathToWin(const std::string& path);
};

} // namespace imog
