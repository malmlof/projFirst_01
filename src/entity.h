// entity.h
#pragma once

#include <cassert> //So we can use assert()
#include <cstdint> //So we have access to uint8_t

struct LevelData;
struct CommandBuffer;

enum Behaviour : uint32_t {
  NONE = 0,
  CAN_MOVE = 1 << 0,
  IS_PLAYER = 1 << 1,
  RESPOND_TO_INPUT = 1 << 2,
  IS_PETRIFIED = 1 << 3,
  CAN_ROTATE = 1 << 4,
  UNPUSHABLE = 1 << 5
};


enum class Direction {
  RIGHT,
  LEFT,
  UP,
  DOWN
};


inline Direction DirectionFromXY(int xDir, int yDir){
  assert(xDir * yDir == 0);
  if(xDir ==  1) { return Direction::RIGHT; }
  if(xDir == -1) { return Direction::LEFT;  }
  if(yDir == 1 ) { return Direction::UP;    }
  else           { return Direction::DOWN;  }
}


enum class ID : uint8_t {
  NONE = 0,
  GROUND = 2,
  WALL = 3,
  DEMON = 1,
  ROCK = 5,
  MEDUSA = 6,
  SIREN = 7,
  GOLEM = 8,
};


struct Position{

  int x;
  int y;
};


struct Entity{
  ID id;
  Direction facing;
  int strength;
  int x;
  int y;
  int x_prev;
  int y_prev;
  float progress_01;
  Behaviour behaviour;
};

bool IsMoving(Entity* e);
bool HasBehaviour(Entity* entity, Behaviour flags);
void InitializeBaseBehaviour(Entity* entity);
void SetBehaviour(Entity* entity, Behaviour flags);
void AddBehaviour(Entity* entity, Behaviour flags);
void RemoveBehaviour(Entity* entity, Behaviour flags);
void PostMove(Entity* entity, LevelData* level, CommandBuffer* commandBuffer);
void PostRotation(Entity* entity, LevelData* level, CommandBuffer* commandBuffer, Direction from, Direction to);
void PreRotation(Entity* entity, LevelData* level, CommandBuffer* commandBuffer, Direction from, Direction to);
