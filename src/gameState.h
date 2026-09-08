// gamestate.h
#pragma once

#include "SDL3/SDL_rect.h"
#include "command.h"
#include "image.h"
#include "imgui/imgui_internal.h"
#include "levels.h"

struct GameData {
  SDL_FRect rect;
  float move_speed;
  Image* fallback;
  Image* ground;
  Image* wall;
  Image* player;
  Image* box;
  Memory::Arena* arena_levels;
  Memory::Arena* arena_entities;
  Memory::Arena* arena_images;
  Memory::Arena* arena_commands;
  CommandBuffer* commandBuffer;
  LevelData* levels;
  int currentLevelIndex;
  int levelCount;
  int currentLevel;
  bool* keys_previous;
  const float* dt;
  ImGuiContext* imGui_context;
  uint32_t command_timestamp;

  LevelData* GetCurrentLevel(){
    return &levels[currentLevel];
  }
  
};
