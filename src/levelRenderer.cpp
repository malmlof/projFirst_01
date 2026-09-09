// levelRenderer.cpp

#include "levelRenderer.h"
// #include "common.h"
#include "rendering.h"
#include <cstdint>
#include <cmath>

void RenderLevel(GameData* gameData, SDL_Renderer* renderer){
  LevelData lvl = gameData->levels[gameData->currentLevel];

  for(int x = 0; x < lvl.w; x++){
    for(int y = 0 ; y < lvl.h; y++){
      uint8_t cellType = lvl.GetCellID(x, y);

      Image* sprite;
      switch(cellType){
        case 2:
          sprite = gameData->ground;
          break;
        case 3:
          sprite = gameData->wall;
          break;
        default:
          sprite = gameData->fallback;
          break;
      }
      RenderSprite_Grid(sprite, &lvl, renderer, &gameData->camera, x, y);
    }
  }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  LevelData lvl = data->levels[data->currentLevel];
  for(int i = 0; i < lvl.entityCount; i++){
    Image* img;
    Entity entity = lvl.entityBuffer[i];
    switch(entity.id){
      case ID::PLAYER:
        img = data->player;
        break;
      case ID::BOX:
        img = data->box;
        break;
      default:
        img = data->fallback;
        break;
    }

    float x_animated = std::lerp(entity.x_prev, entity.x, entity.progress_01);
    float y_animated = std::lerp(entity.y_prev, entity.y, entity.progress_01);

    RenderSprite_Grid(img, &lvl, renderer, &data->camera, x_animated, y_animated);
  }
}


