
#include "Object.h"

void Object::changeFrameIfNeeded(std::chrono::time_point<std::chrono::system_clock> now) {
  using namespace std::literals;

  if (sprite.getFramesCount() <= 1)
    return;

  if (now - lastUpdateTime > 1s / sprite.getFPS()) {
    spriteFrameIndex = (spriteFrameIndex + 1) % sprite.getFramesCount();
    lastUpdateTime = now;
  }
}
