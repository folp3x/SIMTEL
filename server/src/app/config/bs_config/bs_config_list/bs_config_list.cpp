#include "bs_config_list.h"

#include <algorithm>
#include <ranges>

#include "common/validator/validator.h"

namespace server {
bool BsConfigList::mmeConfigExists(unsigned int id) const {
  return std::ranges::any_of(
      mmeConfigs, [id](const MmeConfig &config) { return config.id == id; });
}

std::unique_ptr<common::BaseJsonInfo> BsConfigList::getJsonRootInfo() {
  auto obj = makeJsonObject();

  obj->addInner(
      "ecgi", makeJsonValue<unsigned int>(&curConfig.id, [](unsigned int ecgi) {
        return common::Validator::isPositiveNumber(ecgi, "ECGI");
      }));

  obj->addInner(
      "mmeId",
      makeJsonValue<unsigned int>(&curConfig.mmeId, [this](unsigned int id) {
        return mmeConfigExists(id)
                   ? ""
                   : "MME with id " + std::to_string(id) + " not exists";
      }));

  obj->addInner("radius",
                makeJsonValue<float>(&curConfig.radius, [](float radius) {
                  return common::Validator::isPositiveNumber(radius, "Radius");
                }));

  obj->addInner("loc", makeJsonArray(&curConfig.loc));

  obj->addInner("maxConnections",
                makeJsonValue<unsigned int>(
                    &curConfig.maxConnections, [](unsigned int connections) {
                      return common::Validator::isPositiveNumber(
                          connections, "Max connections");
                    }));

  auto root = makeJsonRepeatObject(std::move(obj),
                                   [this]() { configs.push_back(curConfig); });

  return root;
}

BsConfigList::BsConfigList(const std::vector<MmeConfig> &mmeConfigs_)
    : mmeConfigs(mmeConfigs_) {}

std::vector<BsConfig> BsConfigList::getConfigs() const { return configs; }
} // namespace server
