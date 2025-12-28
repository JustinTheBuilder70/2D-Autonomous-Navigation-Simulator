#include "Prototype5-MyRemake.cpp"
#include <cstdlib> 

using namespace std;



/*For the most part I follwed the implementation on this website
https://www.redblobgames.com/pathfinding/a-star/implementation.html#cpp-dijkstra
This includes the simplest pathfinding algorithm as well as the weighted algorithm
or dijkstras algo.*/

Brain::Brain() {}

void AI::updateMemory(const GameState &state) {
    int p_h = state.pos[0];
    int p_w = state.pos[1];
    

    int vision_h = state.vision.size();
    if (vision_h == 0) return;
    int vision_w = state.vision[0].size();

    int rel_p_r = -1, rel_p_c = -1;


    for(int r = 0; r < vision_h; ++r) {
        for(int c = 0; c < vision_w; ++c) {
            char cell = state.vision[r][c];
            if (cell == '^' || cell == 'v' || cell == '<' || cell == '>') {
                rel_p_r = r;
                rel_p_c = c;
                break;
            }
        }
    }

    if (rel_p_r == -1) return; 

    int start_h = p_h - rel_p_r;
    int start_w = p_w - rel_p_c;

    for (int r = 0; r < vision_h; ++r) {
        for (int c = 0; c < vision_w; ++c) {
            int global_h = start_h + r;
            int global_w = start_w + c;
            char seen = state.vision[r][c];
            
            if (seen == '^' || seen == 'v' || seen == '<' || seen == '>') {
               
                if (memory.find({global_h, global_w}) == memory.end()) {
                    memory[{global_h, global_w}] = ' ';
                }
            } else {
                memory[{global_h, global_w}] = seen;
            }
        }
    }
}


vector<pair<int, int>> AI::findPath(pair<int, int> start, pair<int, int> goal, const GameState &state) {
    queue<pair<int, int>> q;
    q.push(start);
    map<pair<int, int>, pair<int, int>> came_from;
    came_from[start] = start;

    bool found = false;
    int dr[] = {-1, 0, 1, 0};
    int dc[] = {0, -1, 0, 1};

    while (!q.empty()) {
        pair<int, int> current = q.front();
        q.pop();

        if (current == goal) {
            found = true;
            break;
        }

        for (int i = 0; i < 4; ++i) {
            pair<int, int> next = {current.first + dr[i], current.second + dc[i]};
            
            if (came_from.count(next)) continue;

         
            if (memory.find(next) != memory.end()) {
                char tile = memory[next];
                if (tile == '+' || tile == 'X' || tile == 'T' || tile == 'D') continue;
                
                came_from[next] = current;
                q.push(next);
            }
        }
    }

    vector<pair<int, int>> path;
    if (!found) return path;

    pair<int, int> curr = goal;
    while (curr != start) {
        path.push_back(curr);
        curr = came_from[curr];
    }
    reverse(path.begin(), path.end());
    return path;
}

pair<int, int> AI::findNearestTarget(pair<int, int> start, char targetType, const GameState &state) {
    queue<pair<int, int>> q;
    q.push(start);
    set<pair<int, int>> visited;
    visited.insert(start);

    int dr[] = {-1, 0, 1, 0};
    int dc[] = {0, -1, 0, 1};
    
    while (!q.empty()) {
        pair<int, int> current = q.front();
        q.pop();

        if (targetType != '?' && memory.count(current) && memory.at(current) == targetType) {
            return current;
        }
        
       
        if (targetType == '?') {
            bool is_frontier = false;
           
            if (memory.count(current) && memory.at(current) == ' ') {
                for (int i = 0; i < 4; ++i) {
                    pair<int, int> n = {current.first + dr[i], current.second + dc[i]};
                    if (memory.find(n) == memory.end()) {
                        return current;
                    }
                }
            }
        }

        
        for (int i = 0; i < 4; ++i) {
            pair<int, int> next = {current.first + dr[i], current.second + dc[i]};
            if (visited.count(next)) continue;
            
            if (memory.find(next) != memory.end()) {
                char tile = memory[next];
               
                if (tile != '+' && tile != 'X' && tile != 'T' && tile != 'D') {
                    visited.insert(next);
                    q.push(next);
                }                                                                                                                   
            }
        }
    }
    return {-1, -1};
}

int Brain::getNextMove(GameState &state) {
    ai.updateMemory(state);
    pair<int, int> pos = {state.pos[0], state.pos[1]};

   
    ai.history.push_back(pos);
    if (ai.history.size() > 10) ai.history.pop_front();

    
    int visit_count = 0;
    for (const auto& p : ai.history) {
        if (p == pos) visit_count++;
    }
    
    if (visit_count > 4) {
        ai.path.clear(); 
        ai.history.clear(); 
        int valid_moves[] = {1, 2, 3, 4};
     
        return valid_moves[rand() % 4]; 
    }


    if (!ai.path.empty()) {
        if (pos == ai.path.back()) ai.path.clear(); 
        else if (ai.path.size() > 0 && pos != ai.path[0] && 
                 abs(pos.first - ai.path[0].first) + abs(pos.second - ai.path[0].second) > 1) {
            
            ai.path.clear(); 
        }
    }

   
    if (ai.path.empty()) {
        
        static int last_stage = -1;
        if (state.stage != last_stage) {
            ai.hasFlag = false;
            last_stage = state.stage;
        }

        pair<int, int> target = {-1, -1};
        vector<pair<int, int>> potentialPath;

        target = ai.findNearestTarget(pos, '0', state);
        if (target.first != -1) {
            potentialPath = ai.findPath(pos, target, state);
            if (!potentialPath.empty()) ai.path = potentialPath;
        }


        if (ai.path.empty() && state.stage == 3) {
            char flagTarget = ai.hasFlag ? 'B' : 'A';
            target = ai.findNearestTarget(pos, flagTarget, state);
            if (target.first != -1) {
                potentialPath = ai.findPath(pos, target, state);
                if (!potentialPath.empty()) {
                    ai.path = potentialPath;
                
                    if (ai.memory[target] == 'A') ai.hasFlag = true;
                    if (ai.memory[target] == 'B') ai.hasFlag = false;
                }
            }
        }

        
        if (ai.path.empty()) {
            target = ai.findNearestTarget(pos, 'w', state);
            if (target.first != -1) {
                potentialPath = ai.findPath(pos, target, state);
                if (!potentialPath.empty()) ai.path = potentialPath;
            }
        }

       
        if (ai.path.empty()) {
            target = ai.findNearestTarget(pos, '?', state);
            if (target.first != -1) {
                ai.path = ai.findPath(pos, target, state);
            }
        }
    }

  
    if (!ai.path.empty()) {
        pair<int, int> next = ai.path[0];
        ai.path.erase(ai.path.begin()); 
        
        int dy = next.first - pos.first;
        int dx = next.second - pos.second;

        if (dy == -1) return 1;
        if (dy == 1)  return 3;
        if (dx == -1) return 2; 
        if (dx == 1)  return 4; 
    }

    return 0;
}