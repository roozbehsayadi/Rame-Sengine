
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

std::shared_ptr<Image> Sprite::getCurrentFrame() const { return this->frames.at(currentFrame); }

std::shared_ptr<Image> Sprite::getCurrentFrameAndProceed() {
  if (frames.size() == 0u)
    return nullptr;

  auto temp = currentFrame;
  currentFrame = (currentFrame + 1) % frames.size();
  return frames.at(temp);
}

std::vector<std::string> Sprite::getFramePaths() const {
  std::vector<std::string> returnValue = {};
  for (auto frame : frames)
    returnValue.push_back(frame->getImagePath());
  return returnValue;
}

void Sprite::proceedFrame() {
  if (frames.size() == 0u)
    return;

  currentFrame = (currentFrame + 1) % frames.size();
}

void Sprite::removeFrame(int index) { frames.erase(frames.begin() + index); }

void Sprite::removeAllFrames() { frames.clear(); }

std::shared_ptr<Image> Sprite::getFirstFrame() const {
  if (frames.size() <= 0)
    return std::make_shared<Image>("");
  return frames.at(0);
}
