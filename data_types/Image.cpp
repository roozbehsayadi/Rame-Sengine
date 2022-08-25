
#include "Image.h"

Image::Image(const Image &other) { this->imagePath = other.getImagePath(); }

const Image &Image::operator=(const Image &other) {
  this->imagePath = other.imagePath;
  return *this;
}

SDL_Texture *Image::renderAndGetTexture(SDL_Renderer *renderer) {
  if (texture == nullptr) {
    texture = IMG_LoadTexture(renderer, imagePath.c_str());
    if (!texture) {
      std::cerr << "Could not open image " + imagePath + ". Error: " << IMG_GetError() << "\n";
      return nullptr;
    }
  }
  return texture;
}

std::pair<int, int> Image::getSize() {
  if (this->texture == nullptr)
    return {0, 0};
  SDL_Point size;
  SDL_QueryTexture(this->texture, nullptr, nullptr, &size.x, &size.y);
  return {size.x, size.y};
}
