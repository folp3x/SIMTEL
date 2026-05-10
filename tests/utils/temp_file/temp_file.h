#pragma once

#include <string>

// временный файл с безопасным удалением в деструкторе
class TempFile {
private:
  std::string path = "";

public:
  TempFile(const std::string &path_, const std::string &content);
  ~TempFile();

  std::string getPath() const;
};
