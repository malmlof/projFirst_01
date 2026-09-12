// levelRenderer.cpp

#include "levelRenderer.h"
#include "entity.h"
#include "rendering.h"
#include "spritelibrary.h"
#include <cstdint>
#include <cmath>

void RenderLevel(GameData* gameData, SDL_Renderer* renderer){
  LevelData level = gameData->levels[gameData->currentLevel];

  for(int x = 0; x < level.w; x++){
    for(int y = 0 ; y < level.h; y++){
      uint8_t cellType = GetCellID(&level, x, y);

      auto entity = (ID)cellType;
      if(entity == ID::NONE){
        continue;
      }
      
      Sprite* sprite = GetSpriteFromID((ID)cellType, gameData->spriteBuffer);
      if(sprite == nullptr){continue;}
      RenderSprite_Grid(sprite, &level, renderer, &gameData->camera, x, y);
    }
  }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  LevelData lvl = data->levels[data->currentLevel];
  for(int i = 0; i < lvl.entityCount; i++){
    Entity entity = lvl.entityBuffer[i];

    if(entity.id == ID::NONE){
      continue;
    }
    
    Sprite* sprite = GetSpriteFromID(entity.id, data->spriteBuffer);

    if(HasBehaviour(&entity, Behaviour::IS_PETRIFIED)){
      sprite = GetSpriteFromID(ID::ROCK, data->spriteBuffer);
    }

    float x_animated = std::lerp(entity.x_prev, entity.x, entity.progress_01);
    float y_animated = std::lerp(entity.y_prev, entity.y, entity.progress_01);

    RenderSprite_Grid(sprite, &lvl, renderer, &data->camera, x_animated, y_animated);
  }
}


