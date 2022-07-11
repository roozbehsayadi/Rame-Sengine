#ifndef __OBJECT_H
#define __OBJECT_H

#include <chrono>
#include <utility>

#include "Sprite.h"

class Object {
public:
  Object(const std::string &name, Sprite &sprite) : name(name), sprite(sprite) {}

  void setSprite(Sprite &sprite) { this->sprite = sprite; }
  void setName(const std::string &name) { this->name = name; }

  const std::string &getName() const { return this->name; }
  const std::string &getSpriteName() const { return this->sprite.getName(); }

private:
  Sprite &sprite;

  std::string name;
};

#endif // __OBJECT_H
