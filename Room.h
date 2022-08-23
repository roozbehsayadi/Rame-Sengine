
#ifndef __ROOM_H
#define __ROOM_H

#include <memory>
#include <vector>

#include "Object.h"

class Room {
public:
  Room(const std::string &name) : name(name) {}

  const std::string &getName() const { return this->name; }

  void insertObject(std::shared_ptr<Object>);
  std::vector<std::shared_ptr<Object>> getObjects() const { return this->objects; }

private:
  std::string name;

  std::vector<std::shared_ptr<Object>> objects;
};

#endif // __ROOM_H
