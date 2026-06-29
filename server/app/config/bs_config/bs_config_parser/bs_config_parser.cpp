#include "bs_config_parser.h"

#include "common/json/info/json_array_info/json_array_info.h"

namespace server {
BsConfigParser::BsConfigParser(const std::vector<MmeConfig> &mme_)
    : mme(mme_) {}

void BsConfigParser::initFields() {
  common::JsonObjectInfo bsConfigObj{""};

  bsConfigObj.addInner(std::make_unique<common::JsonFieldInfo<unsigned int>>(
      "ecgi", [this](unsigned int id) { curConfig.id = id; }));

  bsConfigObj.addInner(std::make_unique<common::JsonFieldInfo<unsigned int>>(
      "mmeId", [this](unsigned int id) { curConfig.mmeId = id; },
      [this](unsigned int id) {
        bool found = true;
        for (const auto &mmeInfo : mme) {
          if (mmeInfo.id == id) {
            found = true;
            break;
          }
        }
        return found ? "" : "MME with id " + std::to_string(id) + " not exists";
      }));

  bsConfigObj.addInner(std::make_unique<common::JsonFieldInfo<float>>(
      "radius", [this](float radius) { curConfig.radius = radius; },
      [](float radius) {
        return (radius > 0) ? "" : "Radius must be a positive number";
      }));

  bsConfigObj.addInner(std::make_unique<common::JsonArrayInfo<
                           float, common::constants::LOCATION_COORDS_COUNT>>(
      "loc", [this](const common::coords_t<> &loc) { curConfig.loc = loc; }));

  bsConfigObj.addInner(std::make_unique<common::JsonFieldInfo<unsigned int>>(
      "maxConnections",
      [this](unsigned int connections) {
        curConfig.maxConnections = connections;
      },
      [](unsigned int connections) {
        return (connections > 0) ? "" : "Max connections cant be 0";
      }));

  addParsedObjectArray("", std::move(bsConfigObj), [this]() {
    configs.push_back(curConfig);
    curConfig = {};
  });
}

std::expected<std::vector<BsConfig>, std::string>
BsConfigParser::parseJson(const nlohmann::json &json) {
  configs.clear();

  auto error = this->parseFields(json);
  if (error) {
    return std::unexpected(*error);
  }

  return configs;
}

std::unique_ptr<BsConfigParser>
BsConfigParser::create(const std::vector<MmeConfig> &mme) {
  auto parser = std::unique_ptr<BsConfigParser>(new BsConfigParser(mme));
  parser->initFields();
  return parser;
}
} // namespace server
