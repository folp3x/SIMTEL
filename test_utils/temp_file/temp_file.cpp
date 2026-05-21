#include "temp_file.h"

#include <filesystem>
#include <fstream>

TempFile::TempFile(const std::string &path_, const std::string &content)
    : path(path_) {
  std::ofstream file(path);
  file << content;
}

TempFile::~TempFile() { std::filesystem::remove(path); }

std::string TempFile::getPath() const { return path; }
