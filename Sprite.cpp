
#include "Sprite.h"

void Sprite::insertFrame(const std::string &imagePath) {
  auto temp = std::make_shared<Image>(imagePath);
  frames.push_back(temp);
}

void Sprite::insertFrame(int index, const std::string &imagePath) {
  auto temp = std::make_shared<Image>(imagePath);
  frames.insert(frames.begin() + index, temp);
}

std::shared_ptr<Image> Sprite::getFrame(int index) const { return this->frames.at(index); }

void Sprite::removeFrame(int index) { frames.erase(frames.begin() + index); }

void Sprite::removeAllFrames() { frames.clear(); }

std::shared_ptr<Image> Sprite::getFirstFrame() const {
  if (frames.size() <= 0)
    return std::make_shared<Image>("");
  return frames.at(0);
}
