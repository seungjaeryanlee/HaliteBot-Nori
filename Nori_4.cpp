#include <fstream>
#include <string>

#include "hlt.hpp"
#include "networking.hpp"

#define INVALID 5

const int MIN_RATIO = 5;

typedef unsigned char Direction;

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

void printMove(std::ofstream& out, hlt::Move move) {
    out << "(x,y): (" << move.loc.x << "," << move.loc.y << ") ";
    switch(move.dir) {
        case STILL:
            out << "STILL" << std::endl;
            break;
        case NORTH:
            out << "NORTH" << std::endl;
            break;
        case SOUTH:
            out << "SOUTH" << std::endl;
            break;
        case EAST:
            out << "EAST" << std::endl;
            break;
        case WEST:
            out << "WEST" << std::endl;
            break;
    }
}

// Check if the block is a border block
bool isBorder(hlt::GameMap& map, const hlt::Location loc, const unsigned char ID) {
    for(Direction dir : CARDINALS) {
        if(map.getSite(loc, dir).owner != ID) {
            return true;
        }
    }

    return false;
}

// Check whether given strength is enough for the block to attack something
bool isStrongEnough(hlt::GameMap& map, const hlt::Location loc, const int strength, const unsigned char ID) {
    for(Direction dir : CARDINALS) {
        const hlt::Site neighbor = map.getSite(loc, dir);
        if(neighbor.owner != ID && neighbor.strength <= strength ) {
            return true;
        }
    }

    return false;
}

// Find direction with the smallest distance to the border
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

// Get how many enemy blocks will be attacked by moving to loc
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

Direction getAttackMoveDir(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    bool canAttack = false;
    int bestDamageRatio = 0;
    int bestProduction = 0;
    Direction bestAttackDir;

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
        return bestAttackDir;
    }

    return INVALID;
}

Direction getComboAttackMoveDir(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    return INVALID;
}

Direction getWaitOneTurnDir(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    if(isStrongEnough(map, loc, map.getSite(loc).strength + map.getSite(loc).production, ID)) {
        return STILL;
    }
    return INVALID;
}

Direction getAssistMoveDir(hlt::GameMap& map, std::vector<std::vector<Direction>>& moveDirList, const hlt::Location& loc, const unsigned char ID) {

    if(map.getSite(loc).strength != 0) {
        for (Direction dir : CARDINALS) {
            hlt::Location neighbor = map.getLocation(loc, dir);
            // If it's adjacent to my border block that hasn't moved already, and moving would make it able to attack
            if(map.getSite(neighbor).owner == ID 
                && isBorder(map, neighbor, ID) 
                && moveDirList[neighbor.y][neighbor.x] == INVALID 
                && !isStrongEnough(map, neighbor, map.getSite(neighbor).strength + map.getSite(neighbor).production, ID)
                && isStrongEnough(map, neighbor, map.getSite(neighbor).strength + map.getSite(neighbor).production + map.getSite(loc).strength, ID)) {
                
                // Make sure neighbor doesn't move
                moveDirList[neighbor.y][neighbor.x] = STILL;
                return dir;
            }
        }
    }

    return INVALID;
}

Direction getDefaultMoveDir(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    // If it's too small, grow
    if(map.getSite(loc).strength < map.getSite(loc).production * MIN_RATIO) {
        return STILL;
    }

    // If it's on border, check if moving to another border makes that border expandable
    if(isBorder(map, loc, ID)) {
        return STILL;
    }

    // If not too large, 1/2 chance to stay still
    if(map.getSite(loc).strength < 100 && rand() % 2) {
        return STILL;
    }

    // Move towards border
    return findNearestBorder(map, loc, ID);
}

int main() {

    srand(time(NULL));

    std::ofstream log;
    log.open("nori.log");

    std::cout.sync_with_stdio(0);

    unsigned char myID;
    hlt::GameMap presentMap;
    getInit(myID, presentMap);
    sendInit("Nori 4");

    std::set<hlt::Move> moves;
    for(int i = 1; ; i++) {
        moves.clear();

        getFrame(presentMap);
        std::vector<std::vector<Direction>> moveDirList = std::vector<std::vector<Direction>>(presentMap.height, std::vector<Direction>(presentMap.width, INVALID));

        log << "Turn " << i << std::endl;

        // Fill all attack moves
        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID) {
                    moveDirList[a][b] = getAttackMoveDir(presentMap, {b, a}, myID);
                }
            }
        }

        // Fill all combo attack moves
        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID && moveDirList[a][b] == INVALID) {
                    moveDirList[a][b] = getComboAttackMoveDir(presentMap, {b, a}, myID);
                }
            }
        }

        // Fill all waiting moves
        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID && moveDirList[a][b] == INVALID) {
                    moveDirList[a][b] = getWaitOneTurnDir(presentMap, {b, a}, myID);
                }
            }
        }

        // Fill all assist moves
        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID && moveDirList[a][b] == INVALID) {
                    moveDirList[a][b] = getAssistMoveDir(presentMap, moveDirList, {b, a}, myID);
                    //printMove(log, {{b, a}, dir});
                }
            }
        }

        // Submit moves, using Default moves if a block is not specified
        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID) {
                    if(moveDirList[a][b] != INVALID) {
                        moves.insert({{b, a}, moveDirList[a][b]});
                    }
                    else {
                        moves.insert({{b, a}, getDefaultMoveDir(presentMap, {b, a}, myID)});
                    }
                }
            }
        }

        sendFrame(moves);
    }

    log.close();
    return 0;
}
