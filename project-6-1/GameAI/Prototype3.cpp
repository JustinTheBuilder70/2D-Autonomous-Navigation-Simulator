//#include "brain.h"
#include <algorithm>
#include <queue>
#include <unistd.h>
#include <map>
#include <vector>
#include <string>
#include <set>
#include <deque> 
#include <algorithm>
#include <iostream>
#include "../Game/game.h"
//#include "printcpp.cpp"
#include <stack>
#include <random>
#include <unordered_set>


struct TileInfo{
  char tile;
  int seen;
};

struct MultiTile{
  char tile;
  std::pair<int, int> pos;

  bool operator==(const MultiTile& other) const {
    return tile == other.tile && pos == other.pos;
  }

};


struct MultiTileHash {
  std::size_t operator()(const MultiTile& mt) const {
    std::size_t h1 = std::hash<char>{}(mt.tile);
    std::size_t h2 = std::hash<int>{}(mt.pos.first);
    std::size_t h3 = std::hash<int>{}(mt.pos.second);

    // Combine the hashes
    return h1 ^ (h2 << 1) ^ (h3 << 2); 
  }
};

struct Haf{
    std::map<std::pair<int, int>, TileInfo> world;
    std::set<std::pair<int, int>> Visited;
    std::queue<std::pair<int, int>> path;
    std::stack<std::pair<int, int>> walked;
    std::unordered_set<MultiTile, MultiTileHash> tiles;

    std::vector<std::pair<int, int>> dirs = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

    
    std::set<char> goals = {'0', 'B', 'A', 'D'};
    std::pair<int, int> pos;

    std::pair<int, int> last = {-1, -1};
    GameState state;


    int index = 0;
    int cyc = 0;
    bool doorOpened = false;


  void updateState(GameState &gamestate) {
    state = gamestate;
    pos = {state.pos[0], state.pos[1]};
  }


  void updateKnowledge() {
    auto &grid = state.vision;
    std::pair<int, int> player = {-1, -1};
    
    for (int y = 0; y < grid.size(); ++y) {
      for (int x = 0; x < grid[y].size(); ++x) {
        char c = grid[y][x];
        
        if (c == '^' || c == 'v' || c == '>' || c == '<') {
          player = {y, x};
        }
      }
    }

    for (int y = 0; y < grid.size(); ++y) {
      for (int x = 0; x < grid[y].size(); ++x) {
        char c = grid[y][x];

        auto [vy, vx] = player;
        int wy = state.pos[0] + (y - vy);
        int wx = state.pos[1] + (x - vx);
        
        if (c != '^' && c != 'v' && c != '>' && c != '<') {
          world[{wy, wx}].seen++;
          world[{wy, wx}].tile = c;
        } 
      }
    }
  }
/*
  void test() {
    auto &grid = state.vision;
    std::pair<int, int> player = {-1, -1};
    
    for (int y = 0; y < grid.size(); ++y) {
      for (int x = 0; x < grid[y].size(); ++x) {
        char c = grid[y][x];
        
        if (c == '^' || c == 'v' || c == '>' || c == '<') {
          player = {y, x};
        }
      }
    }

    for (int y = 0; y < grid.size(); ++y) {
      for (int x = 0; x < grid[y].size(); ++x) {
        char c = grid[y][x];
        

        auto [vy, vx] = player;
        int wy = state.pos[0] + (y - vy);
        int wx = state.pos[1] + (x - vx);
        
        if (c != '^' && c != 'v' && c != '>' && c != '<') {
          MultiTile t;
          t.tile = c;
          t.pos = {wy, wx};
          
          if (!tiles.count(t)) {
            tiles.insert(t);
          }
        } 
      }
    }
  }


  void reveal() {
    test();

    std::set<std::pair<int, int>> fake;
    std::pair<int, int> last;


    for (auto cord : fake) {
      //print("Test cord", cord);
    }

  }

  bool walkable(int wy, int wx) {
    auto tile = world.find({wy, wx});
   
    if (tile == world.end()) return false;
    if (tile->second.tile == '+') return false;
    if (tile->second.tile == 'D' && !doorOpened) return false; 

    return true;
  }
*/

  std::vector<std::pair<int, int>> frontier() {
    auto [py, px] = pos;
    std::vector<std::pair<int, int>> neighbors;
    for (const auto [dy, dx] : dirs) {
      int ny = py + dy;
      int nx = px + dx;
      if (Visited.count({ny, nx})) continue; 
      if (walkable(ny, nx)) neighbors.push_back({ny, nx});
      //print("Checking:", ny, nx, "Tile:", 
       // world[{ny, nx}].tile,
        //"seen",
       // world[{ny, nx}].seen
      //);  
    }
    return neighbors;
  }


  void walk() {
    std::vector<std::pair<int, int>> neighbors = frontier();

    if (!neighbors.empty()) {
      path.push(neighbors[0]);
      return;
    } 

  }

  
  void leave() {
    std::pair<int, int> goal = {8, 17};

    if (walked.empty()) return;

    std::pair<int, int> cur = walked.top();
    while (!walked.empty()) {
    
      if (std::abs(cur.first - goal.first) + std::abs(cur.second - goal.second) == 1) {
        path.push(cur); 
        path.push(goal);  
        return;
      }

      //print(cur);
      path.push(cur);
      walked.pop();

      if (walked.empty()) break;
      cur = walked.top();
    } 
  }
  

  int move(GameState &state) {
    updateState(state);
    updateKnowledge();
    walk();

    auto [py, px] = pos;
    Visited.insert({py, px});
    walked.push({py, px});

    int move = 0;

    if (path.empty()) {
      leave();
    }

    auto [ny, nx] = path.front();

    path.pop();
    int npy = ny - py;
    int npx = nx - px;    
    
    if (npy == 0 && npx == 1) move = 4;
    if (npy == 1 && npx == 0) move = 3;
    if (npy == 0 && npx == -1) move = 2;
    if (npy == -1 && npx == 0) move = 1;

   
    return move;
  }

};


class Brain {

  public:

    Haf h;

    Brain() {};
    int getNextMove(GameState &state) {
  
      return h.move(state);
      
    }


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



