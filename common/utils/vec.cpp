#include "vec.h"

namespace common {
// формирует строку содержащую элементы вектора
std::string toStr(const std::vector<double> &vec) {
  std::string str = "[";
  for (int i = 0; i < vec.size() - 1; ++i) {
    str += std::to_string(vec[i]) + ", ";
  }
  str += std::to_string(vec[vec.size() - 1]) + "]";
  return str;
}
} // namespace common
