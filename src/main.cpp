// main.cpp
#include <windows.h>
#include <fileapi.h>
#include <cstdio>
#include <fstream>
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "common.h"
#include "arena.h"
#include "gameState.h"

SDL_Window* window;
SDL_Renderer* renderer;

uint64_t NOW = 0;
uint64_t PREV = 0;

constexpr const char* NAME_OF_DLL = "Heartburner_game.dll";
constexpr const char* NAME_OF_TEMP_DLL = "Heartburner_temp.dll";

typedef void (*Function_Initialize) (GameData* data, SDL_Window* window, SDL_Renderer* renderer);
typedef bool (*Function_HandleEvents) (GameData* data, SDL_Event event);
typedef void (*Function_Update) (GameData* data, float dt);
typedef void (*Function_Draw) (GameData* data, SDL_Renderer* renderer);
typedef void (*Function_OnQuit) (SDL_Renderer* renderer);

constexpr const char* NAME_OF_FUNC_INIT = "Initialize";
constexpr const char* NAME_OF_FUNC_HANDLE_EVENT = "HandleEvents";
constexpr const char* NAME_OF_FUNC_UPDATE = "Update";
constexpr const char* NAME_OF_FUNC_DRAW = "Draw";
constexpr const char* NAME_OF_FUNC_QUIT = "OnQuit";

struct DLL_INFO{
    HMODULE dll;
    FILETIME timestamp;
    Function_Initialize initialize;
    Function_HandleEvents handleEvents;
    Function_Update update;
    Function_Draw draw;
    Function_OnQuit quit;
};

FILETIME GetTimestamp(){
    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(NAME_OF_DLL, &data);
    FILETIME time_of_last_change = data.ftLastWriteTime;
    FindClose(handle);
    return time_of_last_change;
}

bool LoadDLL(DLL_INFO* info, int depth = 0){
    if(depth > 20){
        printf("failed to write temp DLL (main.cpp) - line %d\n", __LINE__);
        return false;
    }
    bool success = CopyFile(NAME_OF_DLL, NAME_OF_TEMP_DLL, false);
    if(!success){
        Sleep(50);
        return LoadDLL(info, depth + 1);
    }

    info->dll = LoadLibrary(NAME_OF_TEMP_DLL);

    if(info->dll == nullptr){
        printf("could not load dll (main.cpp) - line %d\n", __LINE__);
        return false;
    }

    info->initialize = (Function_Initialize)GetProcAddress(info->dll, NAME_OF_FUNC_INIT);
    info->handleEvents = (Function_HandleEvents)GetProcAddress(info->dll, NAME_OF_FUNC_HANDLE_EVENT);
    info->update = (Function_Update)GetProcAddress(info->dll, NAME_OF_FUNC_UPDATE);
    info->draw = (Function_Draw)GetProcAddress(info->dll, NAME_OF_FUNC_DRAW);
    info->quit = (Function_OnQuit)GetProcAddress(info->dll, NAME_OF_FUNC_QUIT);
    info->timestamp = GetTimestamp();

    return true;

}

void UnloadDLL(DLL_INFO* info){
    FreeLibrary(info->dll);
    info->dll = nullptr;
    DeleteFile(NAME_OF_TEMP_DLL);
    
}

void* AllocateGameMemory(){
    void* blob = malloc(GAME_MEMORY_ALLOWANCE);
    if(blob == nullptr){
        printf("fatal error: could not allocate memory(main.cpp) - line %d\n", __LINE__);
        return nullptr;
    }

    printf("memory succesfully allocated (main.cpp) - line %d\n", __LINE__);
    return blob;
}

void SDL_Setup(){
    SDL_Init(SDL_INIT_EVENTS);
    window = SDL_CreateWindow("pilot", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
}

void CalculateDeltaTime(float& dt){
    NOW = SDL_GetTicksNS();
    dt = NOW - PREV;
    dt = SDL_NS_TO_SECONDS(dt);
    PREV = NOW;
}

void DLL_CheckStatus(DLL_INFO* dll){
    FILETIME timestamp = GetTimestamp();
    bool is_timestamp_changed = CompareFileTime(&dll->timestamp, &timestamp) != 0;
    if(is_timestamp_changed){
        UnloadDLL(dll);
        LoadDLL(dll);
    }
}


void CalculateRemainingFrameTime_MS(double* milliseconds){
        Uint64 frame_end_time_ns = SDL_GetTicksNS();
        double frame_time_spent_ns = frame_end_time_ns - PREV;
        double frame_time_spent_ms = frame_time_spent_ns / 1e6;
        *milliseconds = FRAME_TIME_MS - frame_time_spent_ms;
}

void StoreGameState(Memory::Arena* arena){
    std::ofstream file("temp_state.bin", std::ios::binary);
    file.write(reinterpret_cast<const char*>(arena->base), arena->size);
}

void RetrieveGameState(Memory::Arena* arena){
    std::ifstream file("temp_state.bin", std::ios::binary);
    file.read(reinterpret_cast<char*>(arena->base), arena->size);
}

int main() {
    void* game_memory = AllocateGameMemory();
    if(game_memory == nullptr){

        return 1;
    }
    Memory::Arena* arena_main = new Memory::Arena();
    Memory::Initialize(arena_main, game_memory, GAME_MEMORY_ALLOWANCE);
    GameData* gameData = (GameData*)Memory::Allocate(arena_main, sizeof(GameData));

    size_t INPUT_ARENA_SIZE = 0;
    INPUT_ARENA_SIZE += sizeof(bool) * SDL_SCANCODE_COUNT * 2;
    INPUT_ARENA_SIZE += sizeof(float) * SDL_SCANCODE_COUNT;
    INPUT_ARENA_SIZE += 128;
    gameData->arena_input = Memory::CreateSubArena(arena_main, INPUT_ARENA_SIZE);

    gameData->input.keys_current = (bool*)Memory::Allocate(gameData->arena_input, sizeof(bool) * SDL_SCANCODE_COUNT);
    gameData->input.keys_previous = (bool*)Memory::Allocate(gameData->arena_input, sizeof(bool) * SDL_SCANCODE_COUNT);
    gameData->input.keys_held_time = (float*)Memory::Allocate(gameData->arena_input, sizeof(float) * SDL_SCANCODE_COUNT);
    int mouseButtonCount = 3;
    gameData->input.mouse_held_time = (float*)Memory::Allocate(gameData->arena_input, sizeof(float) * mouseButtonCount);

    int SPRITE_COUNT = 256;
    size_t IMAGE_ARENA_SIZE = sizeof(Sprite) * SPRITE_COUNT;
    gameData->arena_images = Memory::CreateSubArena(arena_main, IMAGE_ARENA_SIZE);
    gameData->spriteBuffer = (Sprite*)Memory::Allocate(gameData->arena_images, sizeof(Sprite) * SPRITE_COUNT);
    gameData->arena_levels = Memory::CreateSubArena(arena_main, MEGABYTES(3));
    gameData->arena_entities = Memory::CreateSubArena(gameData->arena_levels, MEGABYTES(1));
    gameData->arena_commands = Memory::CreateSubArena(gameData->arena_levels, MEGABYTES(1));

    gameData->input.keys_previous = (bool*)Memory::Allocate(gameData->arena_levels, sizeof(bool) * SDL_SCANCODE_COUNT);

    //Allocate the pointer (array) of levels
    gameData->levelCount = 5;
    gameData->levels = (LevelData*)Memory::Allocate(gameData->arena_levels, sizeof(LevelData) * gameData->levelCount);

    gameData->commandBuffer = (CommandBuffer*)Memory::Allocate(arena_main, sizeof(CommandBuffer));
    gameData->commandBuffer->capacity = 20000;
    size_t COMMAND_SIZE = sizeof(AnyCommand) * gameData->commandBuffer->capacity;
    gameData->commandBuffer->allCommands = (AnyCommand*)Memory::Allocate(gameData->arena_commands, COMMAND_SIZE);

    gameData->input_buffer_capacity = 50;
    size_t RING_BUFFER_SIZE = sizeof(Position) * gameData->input_buffer_capacity;
    gameData->input_buffer = (Position*)Memory::Allocate(gameData->arena_levels, RING_BUFFER_SIZE);

    DLL_INFO dll;
    bool dll_successfully_loaded = LoadDLL(&dll);

    if(dll_successfully_loaded == false){

        return 2;
    }

    MMRESULT result = timeBeginPeriod(1);
        if(result == TIMERR_NOCANDO){
            Sleep(2000);
            return 3;
        }

    SDL_Setup();
    dll.initialize(gameData, window, renderer);

    bool running = true;
    float dt;
    gameData->dt = &dt;

    while(running){

        DLL_CheckStatus(&dll);

        CalculateDeltaTime(dt);
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            running = dll.handleEvents(gameData, event);
            if(running == false){
                break;
            }
            
            if(event.type == SDL_EVENT_QUIT){
                running = false;
                break;
            }
            
            if(event.type == SDL_EVENT_KEY_DOWN){
                if(event.key.key == SDLK_F9){
                    StoreGameState(arena_main);
                }
                if(event.key.key == SDLK_F10){
                    RetrieveGameState(arena_main);
                }
            }
        }

        gameData->input.keys_current = SDL_GetKeyboardState(nullptr);
        gameData->input.mouse_current = SDL_GetMouseState(&gameData->input.mouse_x, &gameData->input.mouse_y);

        dll.update(gameData, dt);

        UpdateKeys(&gameData->input, dt);
        UpdateMouse(&gameData->input, dt);

        dll.draw(gameData, renderer);

        double time_to_sleep_ms;
        CalculateRemainingFrameTime_MS(&time_to_sleep_ms);

        if(time_to_sleep_ms > 0){
            if(time_to_sleep_ms > 1){
                SDL_Delay(time_to_sleep_ms -1);
            }
            while (time_to_sleep_ms > 0){
                CalculateRemainingFrameTime_MS(&time_to_sleep_ms);
            }
        }
        else{
        printf("missed frame  - line %d\n", __LINE__);
        }
    }

    dll.quit(renderer);
    SDL_Quit();

    return 0;
}







