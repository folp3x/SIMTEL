#pragma once

namespace server {
template <typename Condition>
std::expected<std::vector<HlrRecord>, std::string>
SimtelRegister::findInHlr(const Condition &condition) {
  try {
    return hlrStorage.get_all<HlrRecord>(condition);
  } catch (const std::exception &e) {
    return std::unexpected("HLR error: " + std::string(e.what()));
  }
}

template <typename Condition>
std::expected<std::vector<EirRecord>, std::string>
SimtelRegister::findInEir(const Condition &condition) {
  try {
    return eirStorage.get_all<EirRecord>(condition);
  } catch (const std::exception &e) {
    return std::unexpected("EIR error: " + std::string(e.what()));
  }
}
} // namespace server
