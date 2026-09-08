// common.h
#pragma once

#define KILOBYTES(n) ((size_t)n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(10);

constexpr int FPS = 240;
const double FRAME_TIME_MS = 1000.0 / FPS;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int UPSCALE_FACTOR = 2;
const int CELL_SIZE_PX = 32 * UPSCALE_FACTOR;

const float MOVE_SPEED = 6.0;

#define DEBUG_PRINT() \
    printf("DEBUG: %s:%d | %s()\n", __FILE__, __LINE__, __FUNCTION__)
    //använd bara: DEBUG_PRINT(); i koden för prints med funktionen och radnummer.
