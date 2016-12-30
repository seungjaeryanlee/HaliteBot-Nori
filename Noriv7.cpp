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

Direction findNearestDirection(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
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

int findStrength(hlt::GameMap& map, const unsigned char ID) {
    int strength = 0;

    for(unsigned short a = 0; a < map.height; a++) {
        for(unsigned short b = 0; b < map.width; b++) {
            if (map.getSite({b, a}).owner == ID) {
                strength += map.getSite({b, a}).strength;
            }
        }
    }

    return strength;
}

hlt::Move getMove(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    bool isBorder = false;

    // If on border, wait until you can attack
    for(Direction dir : CARDINALS) {
        const hlt::Site neighborSite = map.getSite(loc, dir);
        if (neighborSite.owner != ID) {
            isBorder = true;
            if(neighborSite.strength < map.getSite(loc).strength) {
                return {loc, dir};
            }
        }
    }
    if(isBorder) {
        return {loc, STILL};
    }

    // If below certain ratio of strength/production, don't move
    if(map.getSite(loc).strength < map.getSite(loc).production * MIN_RATIO) {
        return {loc, STILL};
    }

    // 1/2 chance to stay still
    if(rand() % 2) {
        return {loc, STILL};
    }

    // Move towards border
    return {loc, findNearestDirection(map, loc, ID)};
}

int main() {

    srand(time(NULL));

    std::ofstream log;
    log.open("nori.log");

    std::cout.sync_with_stdio(0);

    unsigned char myID;
    hlt::GameMap presentMap;
    getInit(myID, presentMap);
    sendInit("Nori 7");

    std::set<hlt::Move> moves;
    for(int i = 1; ; i++) {
        moves.clear();

        getFrame(presentMap);

        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID) {
                    moves.insert(getMove(presentMap, {b, a}, myID));
                }
            }
        }

        sendFrame(moves);   
    }

    log.close();
    return 0;
}
