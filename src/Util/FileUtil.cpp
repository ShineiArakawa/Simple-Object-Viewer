#include <Util/FileUtil.hpp>

std::string FileUtil::join(const std::string basePath, const std::string additional) {
  tPath path_basePath = generic_fs::absolute(tPath(basePath));
  return (path_basePath / tPath(additional)).string();
}

std::string FileUtil::absPath(const std::string path) { return generic_fs::absolute(tPath(path)).string(); }

std::string FileUtil::dirPath(const std::string path) { return generic_fs::absolute(tPath(path)).parent_path().string(); }

std::string FileUtil::baseName(const std::string path) { return generic_fs::absolute(tPath(path)).filename().string(); }

void FileUtil::mkdirs(const std::string path) {
  tPath path_basePath = generic_fs::absolute(tPath(path));
  generic_fs::create_directories(path_basePath);
}

bool FileUtil::exists(const std::string path) { return generic_fs::exists(generic_fs::absolute(tPath(path))); }

bool FileUtil::isAbsolute(const std::string path) { return tPath(path).is_absolute(); }
