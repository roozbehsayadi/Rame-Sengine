
#include "Image.h"

SDL_Texture *Image::getTexture(SDL_Renderer *renderer) {
  if (texture == nullptr) {
    texture = IMG_LoadTexture(renderer, imagePath.c_str());
    if (!texture) {
      std::cerr << "Could not open image " + imagePath + ". Error: " << IMG_GetError() << "\n";
      return nullptr;
    }
  }
  return texture;
}
