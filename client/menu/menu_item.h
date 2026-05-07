#pragma once

#include "common/app/menu/menu_item.h"

#include <string>
#include <vector>

namespace client {
class MenuItemInvalid : public common::MenuItem {
private:
  std::string error = "";

public:
  explicit MenuItemInvalid(const std::string &error_);
  std::string getName() const override;
  std::string getError() const;
};

class MenuItemUnknown : public common::MenuItem {
public:
  std::string getName() const override;
};

class MenuItemExit : public common::MenuItem {
public:
  std::string getName() const override;
};

class MenuItemActive : public common::MenuItem {
private:
  bool active = false;

public:
  explicit MenuItemActive(bool active_);
  std::string getName() const override;
  bool getActive() const;
};

class MenuItemMove : public common::MenuItem {
private:
  std::vector<double> coords = {};

public:
  explicit MenuItemMove(const std::vector<double> &coords_);
  std::string getName() const override;
  std::vector<double> getCoords() const;
};

class MenuItemProtocol : public common::MenuItem {
private:
  std::string protocol = "";

public:
  explicit MenuItemProtocol(const std::string &protocol_);
  std::string getName() const override;
  std::string getProtocol() const;
};

} // namespace client
