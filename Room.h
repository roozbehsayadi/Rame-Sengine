
#ifndef __ROOM_H
#define __ROOM_H

#include <memory>
#include <vector>

#include "ObjectInstance.h"

class Room {
public:
  Room(const std::string &name) : name(name) {}

  const std::string &getName() const { return this->name; }

  void insertObject(std::shared_ptr<ObjectInstace>);
  std::vector<std::shared_ptr<ObjectInstace>> getObjects() const { return this->objects; }

private:
  std::string name;

  std::vector<std::shared_ptr<ObjectInstace>> objects;
};

#endif // __ROOM_H
