// image.cpp
#include <cassert>
#include <string>
#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"
#include "image.h"
#include "arena.h"
#include <cstdio>
using namespace std;

const char* DIRECTORY = "assets/sprites/";
const char* FALLBACK = "assets/sprites/fallback.png";

Image* AssetManagement::LoadSprite(Memory::Arena* arena, SDL_Renderer* renderer, const char* name){
  string path = DIRECTORY;
  path = path.append(name);

  SDL_Surface* surface = IMG_Load(path.c_str());

  if(surface == nullptr){
    surface = IMG_Load(FALLBACK);
  }
  assert(surface != nullptr);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  Image* img = (Image*)Memory::Allocate(arena, sizeof(Image));

if (img == nullptr){
  printf("Arena har slut på minne");
  SDL_DestroyTexture(texture);
  SDL_DestroySurface(surface);
  return nullptr;
  }

  img->texture = texture;
  if (texture == nullptr){
    printf("Texture == Null\n");
    return nullptr;
  }
  img->height = texture->h;
  img->width = texture->w;
  SDL_DestroySurface(surface);

  return img;
}
