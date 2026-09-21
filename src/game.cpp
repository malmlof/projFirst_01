// game.cpp
// 
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"

#include "game.h"
#include "arena.h"
#include "common.h"
#include "entity.h"
#include "levelRenderer.h"
#include "rendering.h"
#include "command.h"
#include "dev_gui.h"
#include "entity.h"
#include "spritelibrary.h"
#include "gameState.h"
#include "imgui/imgui.h"
#include "input.h"
#include "leveleditor.h"
#include "levels.h"

extern "C" {

void InitializeGame(Gameplay* gameplay, Arena* arena_levels){
  assert(gameplay->initialized == false);
  gameplay->currentLevel = 2;
  CreateLevel(arena_levels, &gameplay->levels[0], "assets/levels/testLevel.tmj");
  CreateLevel(arena_levels, &gameplay->levels[1], "assets/levels/testLEvel_box.tmj");
  CreateLevel(arena_levels, &gameplay->levels[2], "assets/levels/first_04.tmj");
  gameplay->initialized = true;
}

void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer){
    DEV::Initialize(window, renderer);
    AssetManagement::LoadAllSprites(data->spriteBuffer, renderer);
    data->imGui_context = ImGui::GetCurrentContext();
    SDL_Texture* blackfade = GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer)->texture;
    SDL_SetTextureBlendMode(blackfade, SDL_BLENDMODE_BLEND);
    InitializeGame(&data->scenes.gameplay, data->arena_levels);
    ChangeScene(data, SCENE_TYPES::GAME);
}


void StartLevel(Gameplay* gameplay, Arena* arena_commands, Arena* arena_entities){
  Reset(arena_commands);
  CreateEntities(&gameplay->levels[gameplay->currentLevel], arena_entities);
}


void ChangeScene(GameData* data, SCENE_TYPES new_scene){
  assert(new_scene != data->scene_current);
  data->scene_previous = data->scene_current;
  data->scene_current = new_scene;
  data->transition.state = data->scene_previous == SCENE_TYPES::NONE ?
    Transition::FadeFrom :
    Transition::FadeTo;
  data->transition.fade_time_elapsed = 0;
  switch (data->scene_current) {
    case SCENE_TYPES::TITLESCREEN:
      data->transition.fade_time_duration = 1;
      break;
    case SCENE_TYPES::MAINMENU:
      break;
    case SCENE_TYPES::GAME:{
        data->transition.fade_time_duration = 0.5f;
        Gameplay* gameplay = &data->scenes.gameplay;
        assert(gameplay->initialized);
        StartLevel(gameplay, data->arena_commands, data->arena_entities);
        break;
      }
    case SCENE_TYPES::CREDITS:
      break;
    case SCENE_TYPES::NONE:
      assert(false);
      break;
  }
}


bool HandleEvents(GameData *data, SDL_Event event){
  DEV::ProcessEvents(&event);

  if(event.type != SDL_EVENT_KEY_DOWN){
    return true;
  }
  if(event.key.key == SDLK_ESCAPE){
    return false;
  }

  return true;
  }


void UpdateTitleScreen(TitleScreen* titlescreen, const float dt){
  
}


void UpdateGame(Gameplay* gameplay, Input* input, const float dt){
  
}


void Update(GameData* data,float dt){

  Gameplay* gameplay = &data->scenes.gameplay;
  TitleScreen* titlescreen = &data->scenes.titlescreen;
  EditorData* editorData = &data->editor_data;
  Transition* transition = &data->transition;

  if(KeyPressed(&data->input, SDL_SCANCODE_F2)){
    editorData->edit_level = !editorData->edit_level;
  }
  
  if(editorData->edit_level){
    EDITOR::Update(&editorData->editor, &data->input, GetCurrentLevel(gameplay), gameplay->commandBuffer);
  }

  if(KeyPressed(&data->input, SDL_SCANCODE_5)){
    ChangeScene(data, SCENE_TYPES::TITLESCREEN);
    return;
  }

  if(transition->state != Transition::Inactive){
    transition->fade_time_elapsed += dt;
    if(transition->fade_time_elapsed >= transition->fade_time_duration){
      transition->fade_time_elapsed = 0;
      switch (transition->state){
        case Transition::Inactive:
          break;
        case Transition::FadeTo:
          break;
        case Transition::FadeFrom:
          transition->state = Transition::Inactive;
          break;
      }
    }
  }

  switch(data->scene_current){
    case SCENE_TYPES::TITLESCREEN:
      UpdateTitleScreen(titlescreen, dt);
      if(AnyKeyPressed(&data->input)){
        if(transition->state == Transition::FadeTo || transition->state == Transition::Inactive){
          ChangeScene(data, SCENE_TYPES::GAME);
        }
      }
      break;
    case SCENE_TYPES::MAINMENU:
      break;
    case SCENE_TYPES::GAME:
      UpdateGame(gameplay, &data->input, dt);
      break;
    case SCENE_TYPES::CREDITS:
      break;
    case SCENE_TYPES::NONE:
      assert(false);
      break;
  }

  if(KeyPressed(&data->input, SDL_SCANCODE_Z) || KeyHeld_ForTime(&data->input, SDL_SCANCODE_Z, UNDO_REPEAT_TIME)){
    ResetKeyHeldTime(&data->input, SDL_SCANCODE_Z);  
    if(KeyHeld(&data->input, SDL_SCANCODE_LSHIFT)){
      Redo(data->scenes.gameplay.commandBuffer, GetCurrentLevel(gameplay));
    }
    else{
      Undo(data->scenes.gameplay.commandBuffer, GetCurrentLevel(gameplay));
    }
}

if(KeyPressed(&data->input, SDL_SCANCODE_RIGHT) || KeyHeld_ForTime(&data->input, SDL_SCANCODE_RIGHT, (1 / MOVE_SPEED) * 1.15)){
  ResetKeyHeldTime(&data->input, SDL_SCANCODE_RIGHT);
  data->scenes.gameplay.input_buffer[data->scenes.gameplay.input_buffer_write_count++ % data->scenes.gameplay.input_buffer_capacity] = {1, 0};
}
else if(KeyPressed(&data->input, SDL_SCANCODE_LEFT) || KeyHeld_ForTime(&data->input, SDL_SCANCODE_LEFT, (1 / MOVE_SPEED) * 1.15)){
  ResetKeyHeldTime(&data->input, SDL_SCANCODE_LEFT);
  data->scenes.gameplay.input_buffer[data->scenes.gameplay.input_buffer_write_count++ % data->scenes.gameplay.input_buffer_capacity] = {-1, 0};
}
else if(KeyPressed(&data->input, SDL_SCANCODE_UP) || KeyHeld_ForTime(&data->input, SDL_SCANCODE_UP, (1 / MOVE_SPEED) * 1.15)){
  ResetKeyHeldTime(&data->input, SDL_SCANCODE_UP);
  data->scenes.gameplay.input_buffer[data->scenes.gameplay.input_buffer_write_count++ % data->scenes.gameplay.input_buffer_capacity] = {0, -1};
}
else if(KeyPressed(&data->input, SDL_SCANCODE_DOWN) || KeyHeld_ForTime(&data->input, SDL_SCANCODE_DOWN, (1 / MOVE_SPEED) * 1.15)){
  ResetKeyHeldTime(&data->input, SDL_SCANCODE_DOWN);
  data->scenes.gameplay.input_buffer[data->scenes.gameplay.input_buffer_write_count++ % data->scenes.gameplay.input_buffer_capacity] = {0, 1};
}

bool are_entities_moving = false;
  for (int i = 0; i < GetCurrentLevel(gameplay)->entityCount; i++){
    Entity* entity = &GetCurrentLevel(gameplay)->entityBuffer[i];
    if(HasBehaviour(entity, CAN_MOVE) && IsMoving(entity)){
      entity->progress_01 += MOVE_SPEED * dt;
      if(entity->progress_01 >= 1){
        entity->progress_01 = 0;
        entity->x_prev = entity->x;
        entity->y_prev = entity->y;
      }
      if(IsMoving(entity)){
        are_entities_moving = true;
      }
    }
  }

  if(are_entities_moving == false){
    if(data->scenes.gameplay.input_buffer_read_count == data->scenes.gameplay.input_buffer_write_count){
      return;
    }

    data->scenes.gameplay.commandBuffer->timestamp += 1;

    for (int i = 0; i < GetCurrentLevel(gameplay)->entityCount; i++){
      Entity* entity = &GetCurrentLevel(gameplay)->entityBuffer[i];
      if(HasBehaviour(entity, Behaviour::IS_PUSHING)){
        RemoveBehaviour(entity, Behaviour::IS_PUSHING);
      }
      
      if(HasBehaviour(entity, (Behaviour)(RESPOND_TO_INPUT | CAN_MOVE))){
        if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){
          continue;
        }
        int xDir = data->scenes.gameplay.input_buffer[data->scenes.gameplay.input_buffer_read_count % data->scenes.gameplay.input_buffer_capacity].x;
        int yDir = data->scenes.gameplay.input_buffer[data->scenes.gameplay.input_buffer_read_count % data->scenes.gameplay.input_buffer_capacity].y;

        Direction new_facing = DirectionFromXY(xDir, yDir);
        if(new_facing != entity->facing){
          RotateCommand rotate(entity, entity->facing, new_facing);
          Push(data->scenes.gameplay.commandBuffer, rotate, GetCurrentLevel(gameplay));
        }
      
        TryMove(entity, GetCurrentLevel(gameplay), data->scenes.gameplay.commandBuffer, xDir, yDir, entity->strength);
      }
    }
    data->scenes.gameplay.input_buffer_read_count++;
  }
 
}

bool TryMove(Entity* mover, LevelData* level, CommandBuffer* cmd_buffer, int xDir, int yDir, int strength){

  if(HasBehaviour(mover, CAN_MOVE) == false){
    return false;
  }

  if(strength < 0){
    return false;
  }


  int test_x = mover->x + xDir;
  int test_y = mover->y + yDir;
  Entity* stepInto_entity = GetEntity(level, test_x, test_y);
  ID stepInto_tile_id = (ID)GetCellID(level, test_x, test_y);
  if(stepInto_entity == nullptr){
    if(stepInto_tile_id == ID::GROUND){
      MoveCommand mv(mover, xDir, yDir);
      Push(cmd_buffer, mv, level);
      return true;
    }
    return false;
  }
  if(HasBehaviour(stepInto_entity, CAN_MOVE) && !HasBehaviour(stepInto_entity, UNPUSHABLE)){
    if(TryMove(stepInto_entity, level, cmd_buffer, xDir, yDir, --strength)){
      MoveCommand mv(mover, xDir, yDir);
      AddBehaviour(mover, Behaviour::IS_PUSHING);
      Push(cmd_buffer, mv, level);
      return true;
    }
  }
  return false;
}

  void DrawScene(GameData* data, SCENE_TYPES scene, SDL_Renderer* renderer){
    switch(scene){
      case SCENE_TYPES::TITLESCREEN:{
          Sprite* background = GetSprite(SPRITE_ID::titlescreen_background, data->spriteBuffer);
          RenderSprite_World(background, renderer, &data->camera, 0, 0);
        }
        break;
      case SCENE_TYPES::MAINMENU:
      case SCENE_TYPES::GAME:
        RenderLevel(data, renderer);
        RenderEntities(data, renderer);
        break;
      case SCENE_TYPES::CREDITS:
        break;
      case SCENE_TYPES::NONE:
        assert(false);
        break;
      }
  }


  void Draw(GameData* data, SDL_Renderer* renderer){

    DEV::PreDraw(data->imGui_context);
    SDL_SetRenderDrawColor(renderer, 74, 128, 128, 255); //Background Color.
    SDL_RenderClear(renderer);

    switch(data->transition.state){
      case Transition::Inactive:
        DrawScene(data, data->scene_current, renderer);
        break;
      case Transition::FadeTo: {
          DrawScene(data, data->scene_previous, renderer);
          float alpha = data->transition.fade_time_elapsed / data->transition.fade_time_duration;
          RenderSprite_World(GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer), renderer, &data->camera, 0, 0, SCREEN_WIDTH,alpha);
          break;
        }
      case Transition::FadeFrom: {
          DrawScene(data, data->scene_current, renderer);
          float alpha = 1 - data->transition.fade_time_elapsed / data->transition.fade_time_duration;
          RenderSprite_World(GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer), renderer, &data->camera, 0, 0, SCREEN_WIDTH,alpha);
          break;
        }
    }

    DEV::Draw(data,renderer);
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }

} //slutet av extern C
