#ifndef __IMAGE_H
#define __IMAGE_H

#include <iostream>
#include <string>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

class Image {
public:
  Image(const std::string &imagePath) : imagePath(imagePath) {}
  ~Image() { SDL_DestroyTexture(texture); }
  Image(const Image &);

  const Image &operator=(const Image &);

  // TODO add copy constructor (bevause of texture)

  const std::string &getImagePath() const { return this->imagePath; }
  SDL_Texture *renderAndGetTexture(SDL_Renderer *);
  SDL_Texture *&getTexture() { return this->texture; }

  std::pair<int, int> getSize();

private:
  std::string imagePath;
  SDL_Texture *texture = nullptr;
};

#endif // __IMAGE_H
