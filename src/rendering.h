// rendering.h
#pragma once
#include "SDL3/SDL_render.h"
#include "camera.h"
#include "levels.h"
#include "spritelibrary.h"


void RenderSprite_World(Sprite* sprite, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1, float alpha = 1, bool flipped = false);
// void RenderSprite_Grid(Sprite* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1, float alpha = 1, bool flipped = false);
void RenderEntity_OnTile(Sprite* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1, float alpha = 1, bool flipped = 1);
void RenderTile_World(Sprite* tileset, int cell_id, LevelData* level, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale, float alpha);
