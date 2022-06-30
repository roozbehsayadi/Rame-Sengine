
#ifndef __ROOM_H
#define __ROOM_H

#include <memory>
#include <vector>

#include "Object.h"

class Room {
public:
  void insertObject(std::shared_ptr<Object>);

private:
  std::vector<std::shared_ptr<Object>> objects;
};

#endif // __ROOM_H
