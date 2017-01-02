#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>

#include "hlt.hpp"
#include "networking.hpp"

const int MIN_RATIO = 5;

typedef unsigned char Direction;

bool isBorder(hlt::GameMap& map, hlt::Location loc, const unsigned char ID) {
    for(Direction dir : CARDINALS) {
        if(map.getSite(loc, dir).owner != ID) {
            return true;
        }
    }

    return false;
}

// Check whether given strength is enough for the block to attack something
bool isStrongEnough(hlt::GameMap& map, hlt::Location loc, int strength, const unsigned char ID) {
    for(Direction dir : CARDINALS) {
        const hlt::Site neighbor = map.getSite(loc, dir);
        if(neighbor.owner != ID && neighbor.strength <= strength ) {
            return true;
        }
    }

    return false;
}

// void initStrengths(hlt::GameMap& map, std::vector<std::vector<int>>& strengths) {
//     for(unsigned short a = 0; a < map.height; a++) {
//         for(unsigned short b = 0; b < map.width; b++) {
//             strengths[b][a] = 0;
//             }
//     }
// }

// bool willSaturate(hlt::GameMap& map, std::vector<std::vector<int>>& strengths, const hlt::Move& move) {
//     hlt::Site init = map.getSite(move.loc);
//     hlt::Location dest = map.getLocation(move.loc, move.dir);

//     if(strengths[dest.y][dest.x] + init.strength > 255 + 50) { return true; }
//     else { return false; }
// }

// void updateStrengths(hlt::GameMap& map, std::vector<std::vector<int>>& strengths, const hlt::Move& move) {
//     hlt::Site init = map.getSite(move.loc);
//     hlt::Location dest = map.getLocation(move.loc, move.dir);

//     strengths[dest.y][dest.x] += init.strength;
// }

// int distanceFromEnemy(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
//     int maxDistance = (map.width < map.height) ? map.width/2 : map.height/2;

//     for(Direction dir : CARDINALS) {
//         unsigned short distance = 0;
//         hlt::Location current = loc;
//         hlt::Site site = map.getSite(current, dir);
//         while(site.owner == ID && distance < maxDistance) {
//             distance++;
//             current = map.getLocation(current, dir);
//             site = map.getSite(current);
//         }

//         if(distance < maxDistance) {
//             maxDistance = distance;
//         }
//     }

//     return maxDistance;
// }

Direction findNearestBorder(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    int maxDistance = (map.width < map.height) ? map.width/2 : map.height/2;
    Direction bestDir = NORTH;

    for(Direction dir : CARDINALS) {

        unsigned short distance = 0;
        hlt::Location current = loc;
        hlt::Site site = map.getSite(current, dir);
        while(site.owner == ID && distance < maxDistance) {
            distance++;
            current = map.getLocation(current, dir);
            site = map.getSite(current);
        }

        if(distance < maxDistance) {
            bestDir = dir;
            maxDistance = distance;
        }
    }

    return bestDir;
}

int getDamageRatio(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    int damageRatio = 0;

    // FIXME: Check validity of this
    if(map.getSite(loc).owner == 0) { damageRatio++; }

    for (Direction dir : CARDINALS) {
        const hlt::Site neighborSite = map.getSite(loc, dir);
        if(neighborSite.owner != ID && neighborSite.owner != 0) {
            damageRatio++;
        }
    }

    return damageRatio;
}


hlt::Move getMove(hlt::GameMap& map, std::vector<std::vector<bool>>& hasMoved, const hlt::Location& loc, const unsigned char ID) {
    bool canAttack = false;
    int bestDamageRatio = 0;
    int bestProduction = 0;
    Direction bestAttackDir;

    // If it can attack, attack!
    for(Direction dir : CARDINALS) {
        
        const hlt::Site neighborSite = map.getSite(loc, dir);
        if (neighborSite.owner != ID) {
            if(neighborSite.strength < map.getSite(loc).strength) {

                int damageRatio = getDamageRatio(map, map.getLocation(loc, dir), ID);
                int production = map.getSite(loc, dir).production;
                canAttack = true;
                // Prioritize direction that inflicts bigger damage
                if(bestDamageRatio < damageRatio) {
                    bestDamageRatio = damageRatio;
                    bestAttackDir = dir;
                }
                // If same damage, choose site with better production
                if(bestDamageRatio == damageRatio && bestProduction < production) {
                    bestProduction = production;
                    bestAttackDir = dir;
                }
            }
        }
    }
    if(canAttack) {
        hasMoved[loc.y][loc.x] = true;
        return {loc, bestAttackDir};
    }

    if(map.getSite(loc).strength != 0) {
        for (Direction dir : CARDINALS) {
            hlt::Location neighbor = map.getLocation(loc, dir);
            // If it's mine and it's a border block, it hasn't moved already, and moving would make it able to attack
            if(map.getSite(neighbor).owner == ID 
                && isBorder(map, neighbor, ID) 
                && !hasMoved[neighbor.y][neighbor.x] 
                && !isStrongEnough(map, neighbor, map.getSite(neighbor).strength, ID)
                && isStrongEnough(map, neighbor, map.getSite(neighbor).strength + map.getSite(loc).strength, ID)) {
                // Unite with the neighbor border
                hasMoved[loc.y][loc.x] = true;
                // Neighbor should not move
                hasMoved[neighbor.y][neighbor.x] = true;
                return {loc, dir};
            }
        }
    }

    // If it's too small, grow
    if(map.getSite(loc).strength < map.getSite(loc).production * MIN_RATIO) {
        return {loc, STILL};
    }

    // If it's on border, check if moving to another border makes that border expandable
    if(isBorder(map, loc, ID)) {
        return {loc, STILL};
    }

    

    // If not too large, 1/2 chance to stay still
    if(map.getSite(loc).strength < 100 && rand() % 2) {
        return {loc, STILL};
    }

    // Move towards border
    return {loc, findNearestBorder(map, loc, ID)};
}

int main() {

    srand(time(NULL));

    std::ofstream log;
    log.open("nori.log");

    std::cout.sync_with_stdio(0);

    unsigned char myID;
    hlt::GameMap presentMap;
    getInit(myID, presentMap);
    sendInit("Nori 2.1");

    std::set<hlt::Move> moves;
    for(int i = 1; ; i++) {
        moves.clear();

        getFrame(presentMap);
        std::vector<std::vector<bool>> hasMoved = std::vector<std::vector<bool>>(presentMap.height, std::vector<bool>(presentMap.width, false));

        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID) {
                    hlt::Move move = getMove(presentMap, hasMoved, {b, a}, myID);
                    moves.insert(move);
                }
            }
        }

        sendFrame(moves);
    }

    log.close();
    return 0;
}
