#ifndef __OBJECT_H
#define __OBJECT_H

#include <chrono>
#include <utility>

#include "RUI/widgets/elements/ScreenObject.h"
#include "Sprite.h"

class Object : public ScreenObject {
public:
  Object(const std::string &slug, Sprite &sprite, std::pair<double, double> position)
      : ScreenObject(slug, sprite.getFirstFrame()->getImagePath(), {position.first, position.second, -1, -1}),
        sprite(sprite), lastUpdateTime(std::chrono::system_clock::now()) {}

  void setSprite(Sprite &sprite) { this->sprite = sprite; }

  // Use like this:
  // auto now = std::chrono::system_clock::now();
  // object.changeFrameIfNeeded(now);
  void changeFrameIfNeeded(std::chrono::time_point<std::chrono::system_clock>);

private:
  Sprite &sprite;

  std::string slug;

  // xScale and yScale of sprite
  // TODO I'll use these later
  std::pair<double, double> scale = {1.0, 1.0};

  int spriteFrameIndex = 0;

  std::chrono::time_point<std::chrono::system_clock> lastUpdateTime;
};

#endif // __OBJECT_H
