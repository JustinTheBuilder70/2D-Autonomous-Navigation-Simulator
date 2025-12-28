

#include "brain.h"
#include <algorithm>
#include <set>
#include <queue>
#include <unistd.h>
#include <map>
#include <iterator> 


Brain::Brain() {};


void nua(auto& x) {
    std::cout << x << "\n";
}

void printp(std::pair<int, int> point) {
    std::cout << point.first << " " << point.second << "\n";
}



void PrintVision(auto g) {
    for (auto& h : g) {
        for (auto& k : h) {
            std::cout << k << "\n";
        }
    }
}


std::vector<std::pair<int, int>> neighbors(int &h, int &w, 
    std::vector<std::vector<char>>& grid) {

    std::vector<std::pair<int, int>> cords;
    
    int H = grid.size();
    int W = grid[0].size();
    
    int moves[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };  

    //GRAB EVERY SINGLE POINT
    //THEN WE SHOULD BE LOOPING TROUGH EACH COORDINATE,
    //IF THE COORDINATE IS NOT EQYAT TO A WALL.
    //THEN PERFORM MOVEMENT.

    for (auto& m : moves) {
        int nh = h + m[0];
        int nw = w + m[1];

        if (nh >= 0 && nh < H && nw >= 0 && nw < W) {
            
                cords.push_back({nh, nw});
            
            
        }
    }

    return cords;

}


void AI::ConstructGraph(std::pair<int, int> &start, std::pair<int, int> &goal, GameState &gamestate) {
    std::queue<std::pair<int,int>> my_NEST;
    
    my_NEST.push(start);  
    graph[start] = {-1, -1};
    
    while (!my_NEST.empty()) {
        auto current = my_NEST.front();
        my_NEST.pop();

        printp(goal);
        if (current == goal) {
            break;
        }
        

        auto grid = neighbors(current.first, current.second, gamestate.vision);

        for (auto& next : grid) {
            if (!graph.count(next)) {
                graph[next] = current;
                my_NEST.push(next);
            } 
        }
    }
}

std::pair<int,int> visionToWorld(int vy, int vx, const GameState &gamestate) {
    int PWY = gamestate.pos[0]; 
    int PWX = gamestate.pos[1];

    int centerY = gamestate.vision.size() / 2;
    int centerX = gamestate.vision[0].size() / 2;

    int wy = PWY + (vy - centerY);
    int wx = PWX + (vx - centerX);
    
    return {wy, wx};
}


std::pair<int,int> worldToVision(int wy, int wx, const GameState &gamestate) {
    int PWY = gamestate.pos[0];
    int PWX = gamestate.pos[1];

    int centerY = gamestate.vision.size() / 2;
    int centerX = gamestate.vision[0].size() / 2;

    int vy = centerY + (wy - PWY);
    int vx = centerX + (wx - PWX);

    return {vy, vx};
}


char oriented(GameState &gamestate) {
    
    int H = gamestate.vision.size();
    int W = gamestate.vision[0].size();
    
    char c;
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            c = gamestate.vision[y][x];
            if (c == 'v' || c == '^' ||c == '<' || c == '>') {
                return c;
            }
                
        }
            
    }
        

    return '\0'; 

}


std::pair<int, int> AI::ghost(std::pair<int, int> start, GameState &gamestate) {
    std::pair<int, int> position;

    int ny = gamestate.pos[0];
    int nx = gamestate.pos[1];

    auto [vy, vx] = worldToVision(ny, nx, gamestate);

    char orient = oriented(gamestate);

    auto& grid = gamestate.vision;

  
     switch (orient) {
        case 'v': {
            
            if (vy + arm >= grid.size()) break;


            if (grid[vy + arm][vx] != '+') {
                ny += arm;
               
            }

            else {
                arm--;
            }

            break;
            
                
        }

        
        case '^': {
            if (vy - arm < 0) break;
            if (grid[vy - arm][vx] != '+') {
                ny -= arm; 
                
            } else {
                arm--;

            }

            break;

        }
        
        case '>': {

            if (vx + arm < 0) break;

            if (grid[vy][vx + arm] != '+') {
                nx += arm;
                
                
            }
            
        }


        case '<': {
            if (vx - arm < 0) break;
                
            if (grid[vy][vx - arm] != '+') {
                nx -= arm;
                arm--;
            }
                
            
            
        }
        


        
    }
    
    nua("ghost call");
    if (arm <= 0) {

        //auto dirs = {{1,0}, {-1,0}, {0, 1}, {0, -1}};
        printf("Here I help");
        arm = 1;

    }

    //for (int y = 0; y)
    //auto point = findPlayerInVision(gamestate);
    //std::cout  << "Player Point Vision " << point.first << " " << point.second << "\n";
    //std::cout  << "Player Point World " << gamestate.pos[0] << " " << gamestate.pos[1] << "\n";
    

    return {ny, nx};
} 
a
void AI::buildpath(GameState &gamestate, auto start, auto goal) {

   if (!graph.count(goal)) {
        return;
    }

    auto current = goal;
    
    while (current != start) {
        
        path.push_back(current);
        current = graph.at(current);
    }


    path.push_back(start);
    std::reverse(path.begin(), path.end());

    
}

std::pair<int, int> AI::getPlayerCord(GameState &gamestate) {
    return{gamestate.pos[0], gamestate.pos[1]};
}

int direction(std::pair<int,int> from, std::pair<int,int> to)  {   
    if (to.first < from.first)return 1;

    if (to.first > from.first) return 3;

    if (to.second > from.second) return 4;
    if (to.second < from.second) return 2;

    return 4;
}



void AI::updateWorldKnowledge(GameState &gamestate) {
    auto &vision = gamestate.vision;
    int H = vision.size();
    int W = vision[0].size();

    for (int vy = 0; vy < H; ++vy) {
        for (int vx = 0; vx < W; ++vx) {
            char tile = vision[vy][vx];
            auto [wy, wx] = visionToWorld(vy, vx, gamestate);
            knownWorld[{wy, wx}] = tile;
        }
    }
}

std::pair<int, int> AI::Explore(GameState &gamestate) {
    auto &vision = gamestate.vision;
    auto player = getPlayerCord(gamestate);
    
    int H = vision.size();
    int W = vision[0].size();

    /*
    char orient = oriented(gamestate);

    if (ptr >= path.size() || path.empty( )) {
        ConstructGraph(player, last,  gamestate);
        buildpath(gamestate, player, last);  
        //path.clear();
        ptr = 1;
    }

    */

    
    //if (player == goal) {
      //  ghost(getPlayerCord(gamestate));
    //}

    for (int vy = 0; vy < H; ++vy) {
        for (int vx = 0; vx < W; ++vx) {
            char tile = vision[vy][vx];
            auto [wy, wx] = visionToWorld(vy, vx, gamestate);
            
            if (tile == '0' || tile == 'A') {
                return {wy, wx};
            }

    
        
        }
    }



    return ghost(getPlayerCord(gamestate), gamestate);
}



int Brain::getNextMove(GameState &gamestate) {
    auto player = ai.getPlayerCord(gamestate);

    ai.last = {ai.Explore(gamestate)};
    // 1. Update knowledge (optional but good)
    ai.updateWorldKnowledge(gamestate);


    if (ai.ptr >= ai.path.size() || ai.path.empty()) {
        ai.path.clear();
        ai.ConstructGraph(player, ai.last,  gamestate);
        ai.buildpath(gamestate, player, ai.last);  
        
        ai.ptr = 1;

    }

    std::pair<int, int> next;
    int move;

    if (ai.ptr < ai.path.size()) {
        next = ai.path[ai.ptr];
        move = direction(player, next);
        ai.ptr++;
        return move;
    }

    next = ai.Explore(gamestate);
    move = direction(player, next);
    return move;

}


/*
int Brain::getNextMove(GameState &gamestate) {
    auto startLocal = findPlayerInVision(gamestate);
    if (startLocal.first < 0) return 0;


    if (ai.path.empty() || ai.ptr >= ai.path.size()) {
        auto goalLocal = ai.next(startLocal, gamestate);
        if (goalLocal.first < 0) {
           
            return 0;
        }

        ai.graph = ConstructGraph(startLocal, gamestate);
        ai.path  = buildpath(ai.graph, startLocal, goalLocal);
        ai.ptr   = 1; 
    }

    return ai.movementEngine(gamestate);

    ai.current = findPlayerInVision(gamestate);
    ai.graph = ConstructGraph(ai.current, gamestate);
    auto goal = ai.next(ai.current, gamestate);

    ai.path = buildpath(ai.graph, gamestate, ai.current, goal);

    int move = ai.movementEngine(gamestate, goal);
    
    //ai.path.clear();
    
    return move;

}
*/