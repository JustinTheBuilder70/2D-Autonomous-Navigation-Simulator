#pragma once 
#include <vector>
#include <string>
#include <map>
#include <set>
#include <deque> 
#include <queue>
#include <algorithm>
#include <iostream>


struct AI {
   
    std::map<std::pair<int, int>, char> memory;

    std::vector<std::pair<int, int>> path;

    std::deque<std::pair<int, int>> history; 
    
    bool hasFlag = false;

    void updateMemory(const GameState &state);

    std::pair<int, int> findNearestTarget(std::pair<int, int> start, char targetType, const GameState &state);
    std::vector<std::pair<int, int>> findPath(std::pair<int, int> start, std::pair<int, int> goal, const GameState &state);
};

struct Haf{
  std::map<std::pair<int, int>, char> world;
  GameState state;

  void updateState(GameState &gamestate) {
    state = gamestate;
  }

  void search() {
    int H = state.vision.size();
    int W = state.vision[0].size();

    for (int y = 0; y < H; ++y) {

      for (int x = 0; x < W; ++x) {
        auto worldp = visionToWorld(y, x);
        if (world.find(worldp) == world.end()) {
          std::cout << y << " " << x << "\n";
          world.insert({worldp, state.vision[y][x]});
        }
      }   
    }
  }
  

  std::pair<int,int> visionToWorld(int vy, int vx) {
    int PWY = state.pos[0]; 
    int PWX = state.pos[1];

    int centerY = state.vision.size() / 2;
    int centerX = state.vision[0].size() / 2;

    int wy = PWY + (vy - centerY);
    int wx = PWX + (vx - centerX);
    
    return {wy, wx};
  }

  int path() {
    auto curr_ = world.begin();
    auto prev_ = world.end();

    int m = 0;
    
    if (prev_ != world.end()) {
      if (curr_->first > prev_->first) m = 3;
    }

    prev_ = curr_;
    ++curr_;
  
    return m;

  }

};

class Brain {

  public:
    AI ai;
    Haf h;
    Brain();
    int getNextMove(GameState &state);


};



// BRAIN_H
/*
#ifndef BRAIN_H
#define BRAIN_H

#include <string>
#include "../Game/game.h"
#include <cstdlib>
#include <ctime>
#include <map>
#include <vector>

// Simple state container for our pathfinding AI.
// All the real logic lives in brain.cpp so the header stays small.
struct AI {
    // What we know about the world so far: maps (row, col) -> tile character.
    // Coordinates use the same (h, w) convention as GameState::pos.
    std::map<std::pair<int,int>, char> world;

    // Current planned path as a sequence of global coordinates.
    std::vector<std::pair<int,int>> path;
    int pathIndex{0};      // index of our current position inside path

    // Flag-capture state (used on the stage with A/B).
    bool hasFlag{false};
    bool knowFlagA{false};
    bool knowFlagB{false};
    std::pair<int,int> flagAPos{0,0};
    std::pair<int,int> flagBPos{0,0};

    // Basic bookkeeping.
    int lastStage{-1};
    bool initialized{false};
};

class Brain
{
public:
    AI ai;
    Brain();                               // Constructor
    int getNextMove(GameState &gamestate); // Returns the next move for the AI
};
*/

