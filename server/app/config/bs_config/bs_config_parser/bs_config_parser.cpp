#include "bs_config_parser.h"

#include "common/json/info/json_array_info/json_array_info.h"
#include "common/validator/validator.h"

namespace server {
BsConfigParser::BsConfigParser(const std::vector<MmeConfig> &mme_)
    : mme(mme_) {}

void BsConfigParser::initFields() {
  auto bsConfigObj = makeParsedObject("");

  bsConfigObj->addInner(makeParsedField<unsigned int>(
      "ecgi", [this](unsigned int id) { curConfig.id = id; }));

  bsConfigObj->addInner(makeParsedField<unsigned int>(
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

  bsConfigObj->addInner(makeParsedField<float>(
      "radius", [this](float radius) { curConfig.radius = radius; },
      [](float radius) {
        return common::Validator::isPositiveNumber(radius, "Radius");
      }));

  bsConfigObj->addInner(
      makeParsedArray<float, common::constants::LocationCoordsCount>(
          "loc",
          [this](const common::coords_t<> &loc) { curConfig.loc = loc; }));

  bsConfigObj->addInner(makeParsedField<unsigned int>(
      "maxConnections",
      [this](unsigned int connections) {
        curConfig.maxConnections = connections;
      },
      [](unsigned int connections) {
        return common::Validator::isPositiveNumber(connections,
                                                   "Max connections");
      }));

  addInfo(makeParsedObjectArray("", std::move(bsConfigObj), [this]() {
    configs.push_back(curConfig);
    curConfig = {};
  }));
}

std::expected<std::vector<BsConfig>, std::string>
BsConfigParser::parseJson(const nlohmann::json &json) {
  configs.clear();

  auto error = parseFields(json);
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
