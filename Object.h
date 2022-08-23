#ifndef __OBJECT_H
#define __OBJECT_H

#include <chrono>
#include <utility>

#include "Sprite.h"

class Object {
public:
  Object(const std::string &name, const std::string &instanceName, Sprite &sprite)
      : name(name), instanceName(instanceName), sprite(sprite) {}

  void setSprite(Sprite &sprite) { this->sprite = sprite; }
  void setName(const std::string &name) { this->name = name; }

  const std::string &getName() const { return this->name; }
  const std::string &getSpriteName() const { return this->sprite.getName(); }
  const std::string &getInstanceName() const { return this->instanceName; }

  Sprite &getSprite() const { return this->sprite; }

private:
  Sprite &sprite;

  std::string name;
  std::string instanceName;
};

#endif // __OBJECT_H
