#pragma once

#include <string>

class TempFile {
private:
  std::string path = "";

public:
  TempFile(const std::string &path_, const std::string &content);
  ~TempFile();

  std::string getPath() const;
};
