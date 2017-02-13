#include <fstream>
#include <string>

#include "hlt.hpp"
#include "networking.hpp"
#include "nori.h"

#define INVALID 5

int MIN_RATIO = 5;

// Returns the least strength of a block adjacent to loc that the bot can attack
int nearestBorderStrength(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    int minDistance = (map.width < map.height) ? map.width/2 : map.height/2;

    int strength = 0;

    for(Direction dir : CARDINALS) {

        int distance = 0;
        hlt::Location current = loc;
        hlt::Site site = map.getSite(current, dir);

        // If on border, distance would be 0.
        for(distance = 0; distance < minDistance; distance++) {
            if(site.owner != ID) { break; }
            current = map.getLocation(current, dir);
            site = map.getSite(current);
        }

        if(distance < minDistance) {
            minDistance = distance;
            strength = site.strength;
        }
    }

    return strength;
}

// Returns minimum distance to the border block from loc
int distanceToBorder(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    int minDistance = (map.width < map.height) ? map.width/2 : map.height/2;

    for(Direction dir : CARDINALS) {

        int distance = 0;
        hlt::Location current = loc;
        hlt::Site site = map.getSite(current, dir);

        // If on border, distance would be 0.
        for(distance = 0; distance < minDistance; distance++) {
            if(site.owner != ID) { break; }
            current = map.getLocation(current, dir);
            site = map.getSite(current);
        }

        if(distance < minDistance) {
            minDistance = distance;
        }
    }

    return minDistance;
}

// Find direction with the smallest distance to the border
Direction findNearestBorder(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    int minDistance = (map.width < map.height) ? map.width/2 : map.height/2;

    Direction bestDir = NORTH;

    for(Direction dir : CARDINALS) {

        int distance = 0;
        hlt::Location current = loc;
        hlt::Site site = map.getSite(current, dir);

        // If on border, distance would be 0.
        for(distance = 0; distance < minDistance; distance++) {
            if(site.owner != ID) { break; }
            current = map.getLocation(current, dir);
            site = map.getSite(current);
        }

        if(distance < minDistance) {
            bestDir = dir;
            minDistance = distance;
        }
    }

    return bestDir;

    // TODO: If bestDir was never changed, do something else?
    // TODO: Stay still or find different border if oversaturation can happen?
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

// Return direction of attack the block at loc should perform or return INVALID
Direction getAttackMoveDir(hlt::GameMap& map, std::vector<std::vector<bool>>& isTarget, const hlt::Location& loc, const unsigned char ID) {
    bool canAttack = false;
    int bestDamageRatio = 0;
    int bestProduction = 0;
    Direction bestAttackDir;

    for(Direction dir : CARDINALS) {
        
        const hlt::Site neighborSite = map.getSite(loc, dir);
        if (neighborSite.owner != ID && !isTarget[map.getLocation(loc, dir).y][map.getLocation(loc, dir).x]) {
            if(neighborSite.strength < map.getSite(loc).strength) {

                int damageRatio = getDamageRatio(map, map.getLocation(loc, dir), ID);
                int production = map.getSite(loc, dir).production;
                canAttack = true;
                // Prioritize direction that inflicts bigger damage
                if(bestDamageRatio < damageRatio) {
                    bestDamageRatio = damageRatio;
                    bestAttackDir = dir;
                    bestProduction = production;
                }
                // If same damage, choose site with better production
                else if(bestDamageRatio == damageRatio && bestProduction < production) {
                    bestProduction = production;
                    bestAttackDir = dir;
                }
            }
        }
    }
    if(canAttack) {
    	isTarget[map.getLocation(loc, bestAttackDir).y][map.getLocation(loc, bestAttackDir).x] = true;
        return bestAttackDir;
    }

    return INVALID;
}

// Return STILL if waiting one turn allows attacking, otherwise returns INVALID
Direction getWaitOneTurnDir(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    if(isStrongEnough(map, loc, map.getSite(loc).strength + map.getSite(loc).production, ID)) {
        return STILL;
    }
    return INVALID;
}

// Return direction of assist the block at loc should perform or return INVALID
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

// Return default action the block loc should perform
Direction getDefaultMoveDir(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    // If it's too small, grow
    // Equal in case that production = 0
    if(map.getSite(loc).strength <= map.getSite(loc).production * MIN_RATIO) {
        return STILL;
    }

    // If it's on border, stay still
    if(isBorder(map, loc, ID)) {
        return STILL;
    }

	if(map.getSite(loc).strength * distanceToBorder(map, loc, ID) > nearestBorderStrength(map, loc, ID)) {
		return findNearestBorder(map, loc, ID);
	}

 	return STILL;
}

int main() {

    srand(time(NULL));

    std::ofstream log;
    log.open("nori5.log");

    std::cout.sync_with_stdio(0);

    unsigned char myID;
    hlt::GameMap presentMap;
    getInit(myID, presentMap);
    sendInit("Nori 5");

    std::set<hlt::Move> moves;

    for(int i = 1; ; i++) {
        moves.clear();

        getFrame(presentMap);
        std::vector<std::vector<Direction>> moveDirList = std::vector<std::vector<Direction>>(presentMap.height, std::vector<Direction>(presentMap.width, INVALID));
        std::vector<std::vector<bool>> isTarget = std::vector<std::vector<bool>>(presentMap.height, std::vector<bool>(presentMap.width, false));

        log << "Turn " << i << std::endl;

        // Fill all attack moves
        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID) {
                    moveDirList[a][b] = getAttackMoveDir(presentMap, isTarget, {b, a}, myID);
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
                }
            }
        }

        // Submit moves, using Default moves if a block is not specified
        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID) {
                    if(moveDirList[a][b] != INVALID) {
                        moves.insert({{b, a}, moveDirList[a][b]});
                        printMove(log, {{b, a}, moveDirList[a][b]});
                    }
                    else {
                        Direction dir = getDefaultMoveDir(presentMap, {b, a}, myID);
                        moves.insert({{b, a}, dir});
                        printMove(log, {{b, a}, dir});
                    }
                }
            }
        }

        sendFrame(moves);
    }

    log.close();
    return 0;
}
