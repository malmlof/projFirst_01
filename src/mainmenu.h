// mainmenu.h
#pragma once

#include "fontlibrary.h"
#include "input.h"


struct GameData;
struct SDL_Renderer;
struct Button;
struct Sprite;
namespace Memory{
  struct Arena;
}

struct MainMenu {
  Button* buttons;
  int button_count;
  int activeButtonIndex;
  Button** activeButtons;
  int activeButtonCount;
  bool initialized;
  Sprite* background_horizon;
  Sprite* background_cloud_back;
  Sprite* background_cloud_front;
  Sprite* background_middle;
  Sprite* background_front;
};

void InitializeMenu(MainMenu* mainmenu, Sprite* spriteBuffer, FontAtlas* font, Memory::Arena* arena_main);
void UpdateMenu(GameData* data);
void DrawMenu(MainMenu* mainmenu,  SDL_Renderer* renderer, Sprite* spriteBuffer, Input* input);
