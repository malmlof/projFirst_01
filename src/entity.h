// entity.h
#pragma once

#include <cassert> //So we can use assert()
#include <cstdint> //So we have access to uint8_t

enum Behaviour : uint32_t {
  NONE = 0,
  CAN_MOVE = 1 << 0,
  IS_PLAYER = 1 << 1,
  RESPOND_TO_INPUT = 1 << 2
};

enum class ID : uint8_t {
  NONE = 0,
  GROUND = 2,
  WALL = 3,
  PLAYER = 1,
  BOX = 5
};

struct Entity{
  //uint8_t id;
  ID id;
  int x;
  int y;
  Behaviour behaviour;

  bool HasBehaviour(Behaviour flags){
    return (behaviour & flags) == flags;
  }

  void SetBehaviour(Behaviour flags){
    behaviour = flags;
  }

  void AddBehaviour(Behaviour flags){
    behaviour = (Behaviour)(behaviour | flags);
  }

  void RemoveBehaviour(Behaviour flags){
    behaviour = (Behaviour)(behaviour & ~flags);
  }

  void InitializeBaseBehaviour(){
    assert(id != ID::NONE);
    switch (id) {
      default:
        SetBehaviour(NONE);
        break;
      case ID::PLAYER:
        SetBehaviour((Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
        break;
      case ID::BOX:
        SetBehaviour((Behaviour)CAN_MOVE);
        break;
    }
  }
};
