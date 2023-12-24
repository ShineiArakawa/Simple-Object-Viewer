#pragma once
#ifndef _FILE_UTIL_H_
#define _FILE_UTIL_H_

#include <filesystem>
#include <string>

namespace generic_fs = std::filesystem;

class FileUtil {
  using tPath = std::filesystem::path;

 public:
  static std::string join(const std::string, const std::string);
  static std::string dirPath(const std::string);
  static std::string baseName(const std::string);
  static std::string absPath(const std::string);
  static std::string cwd();
  static void mkdirs(const std::string);
  static bool exists(const std::string);
  static bool isAbsolute(const std::string);
};

#endif
