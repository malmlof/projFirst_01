// gamestate.h
#pragma once

#include "arena.h"
#include "camera.h"
#include "command.h"
#include "entity.h"
#include "imgui/imgui_internal.h"
#include "levels.h"
#include "input.h"
#include "spritelibrary.h"
#include "leveleditor.h"


struct Gameplay {
  CommandBuffer* commandBuffer;
  LevelData* levels;
  int levelCount;
  int currentLevel;
  Position* input_buffer;
  int input_buffer_capacity;
  int input_buffer_write_count;
  int input_buffer_read_count;
  bool initialized;
};

struct MainMenu {
  
};

struct Transition {
  enum States {
    Inactive,
    FadeTo,
    FadeFrom
  };
  States state;
  float fade_time_elapsed;
  float fade_time_duration = 1;
};

struct TitleScreen {
  enum States {
    Inactive,
    FadeTo,
    FadeFrom
  };
  States state;
  float fade_time_elapsed;
  float fade_time_duration = 1;
};

struct Credits {
  
};



struct Scenes{
    Gameplay gameplay;
    MainMenu mainMenu;
    TitleScreen titlescreen;
    Credits credits;
  };

enum class SCENE_TYPES : uint8_t{
  NONE,
  TITLESCREEN,
  MAINMENU,
  GAME,
  CREDITS,
};

struct EditorData{
  float* fps_buffer;
  int fps_buffer_count;
  bool edit_level;
  Editor editor;
  int fps_buffer_index;
};


struct GameData {
  SDL_FRect rect;
  float move_speed;
  Sprite* spriteBuffer;

  SCENE_TYPES scene_current;
  SCENE_TYPES scene_previous;
  Scenes scenes;
  Transition transition;
  EditorData editor_data;
  
  Memory::Arena* arena_main;
  Memory::Arena* arena_input;
  Memory::Arena* arena_levels;
  Memory::Arena* arena_entities;
  Memory::Arena* arena_images;
  Memory::Arena* arena_commands;
  Memory::Arena* arena_scratch;

  const float* dt;
  ImGuiContext* imGui_context;

  Input input;
  Camera camera;
  bool edit_level;
  Editor editorData;

  
};


inline  LevelData* GetCurrentLevel(Gameplay* game){
    return &game->levels[game->currentLevel];
  }


