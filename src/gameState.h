// gamestate.h
#pragma once

#include "SDL3/SDL_rect.h"
#include "camera.h"
#include "command.h"
#include "imgui/imgui_internal.h"
#include "levels.h"
#include "input.h"
#include "spritelibrary.h"

struct GameData {
  SDL_FRect rect;
  float move_speed;
  Sprite* spriteBuffer;
  Memory::Arena* arena_levels;
  Memory::Arena* arena_entities;
  Memory::Arena* arena_images;
  Memory::Arena* arena_commands;
  CommandBuffer* commandBuffer;
  LevelData* levels;
  int currentLevelIndex;
  int levelCount;
  int currentLevel;
  const float* dt;
  ImGuiContext* imGui_context;
  uint32_t command_timestamp;
  Position* input_buffer;
  int input_buffer_capacity;
  int input_buffer_write_count;
  int input_buffer_read_count;
  Input input;
  Arena* arena_input;
  Camera camera;

  LevelData* GetCurrentLevel(){
    return &levels[currentLevel];
  }
  
};
