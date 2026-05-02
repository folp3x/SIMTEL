#pragma once

#include <string>
#include <vector>

class MenuItem {
public:
  virtual std::string getName() const = 0;
  virtual ~MenuItem() = default;
};

class MenuItemInvalid : public MenuItem {
private:
  std::string error = "";

public:
  explicit MenuItemInvalid(const std::string &error_);
  std::string getName() const override;
  std::string getError() const;
};

class MenuItemUnknown : public MenuItem {
public:
  std::string getName() const override;
};

class MenuItemExit : public MenuItem {
public:
  std::string getName() const override;
};

class MenuItemActive : public MenuItem {
private:
  bool active = false;

public:
  explicit MenuItemActive(bool active_);
  std::string getName() const override;
  bool getActive() const;
};

class MenuItemMove : public MenuItem {
private:
  std::vector<double> coords = {};

public:
  explicit MenuItemMove(const std::vector<double> &coords_);
  std::string getName() const override;
  std::vector<double> getCoords() const;
};

class MenuItemProtocol : public MenuItem {
private:
  std::string protocol = "";

public:
  explicit MenuItemProtocol(const std::string &protocol_);
  std::string getName() const override;
  std::string getProtocol() const;
};
