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

hlt::Move getMove(hlt::GameMap& map, const hlt::Location& loc, const unsigned char ID) {
    bool isBorder = false;
    bool canAttack = false;
    int bestDamageRatio = 0;
    int bestProduction = 0;
    Direction bestAttackDir;

    // If on border, wait until you can attack
    for(Direction dir : CARDINALS) {
        const hlt::Site neighborSite = map.getSite(loc, dir);
        if (neighborSite.owner != ID) {
            isBorder = true;
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
        return {loc, bestAttackDir};
    }
    if(isBorder) {
        return {loc, STILL};
    }

    // If below certain ratio of strength/production, don't move
    if(map.getSite(loc).strength < map.getSite(loc).production * MIN_RATIO) {
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
    sendInit("Nori 2.0");

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
