// rendering.h
#pragma once
#include "SDL3/SDL_render.h"

struct Image; // forward declaration

void RenderSprite(Image* sprite, SDL_Renderer* renderer, int xPos, int yPos, float scale = 1);

