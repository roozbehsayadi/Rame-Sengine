#ifndef __SPRITE_H
#define __SPRITE_H

#include <memory>
#include <vector>

#include "data_types/Image.h"

class Object;

class Sprite {

  friend class Object;

public:
  Sprite() {}
  Sprite(const std::string &name, double FPS = 0.0, std::vector<std::string> frames = {}) : FPS(FPS), name(name) {
    if (!frames.empty()) {
      for (auto imagePath : frames) {
        this->insertFrame(imagePath);
      }
    }
  }

  void insertFrame(const std::string &);
  void insertFrame(int, const std::string &);

  std::shared_ptr<Image> getFrame(int) const;
  std::shared_ptr<Image> getCurrentFrame() const;
  std::shared_ptr<Image> getCurrentFrameAndProceed();
  std::vector<std::string> getFramePaths() const;

  void proceedFrame();

  void removeFrame(int);
  void removeAllFrames();

  std::shared_ptr<Image> getFirstFrame() const;
  int getFramesCount() const { return this->frames.size(); }

  void setFPS(double FPS) { this->FPS = FPS; }

  double getFPS() const { return this->FPS; }
  const std::string &getName() const { return this->name; }

private:
  // TODO get rid of these pointless pointers
  std::vector<std::shared_ptr<Image>> frames;

  int currentFrame = 0;

  double FPS;
  std::string name;
};

#endif
