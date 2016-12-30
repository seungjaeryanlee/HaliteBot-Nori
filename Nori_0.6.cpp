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

Direction findNearestDirection(hlt::GameMap& map, hlt::Location loc, const unsigned char ID) {
    int maxDistance = (map.width < map.height) ? map.width/2 : map.height/2;
    Direction bestDir = NORTH;

    for(Direction d : CARDINALS) {
        unsigned short distance = 0;
        hlt::Location current = loc;
        hlt::Site site = map.getSite(current, d);
        while(site.owner == ID && distance < maxDistance) {
            distance++;
            current = map.getLocation(current, d);
            site = map.getSite(current);
        }

        if(distance < maxDistance) {
            bestDir = d;
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

int main() {

    std::ofstream log;
    log.open("nori.log");

    srand(time(NULL));

    std::cout.sync_with_stdio(0);

    unsigned char myID;
    hlt::GameMap presentMap;
    getInit(myID, presentMap);
    sendInit("Nori 0.6");

    std::set<hlt::Move> moves;
    for(int i = 1; ; i++) {
        moves.clear();

        // log << "Turn " << i << ": " << findStrength(presentMap, myID) << " strength" << std::endl;

        getFrame(presentMap);

        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID) {
                    bool isMoved = false;
                    bool isBorder = false;

                    // If on border, wait until you can attack
                    for(Direction dir : CARDINALS) {
                        const hlt::Site neighborSite = presentMap.getSite({b, a}, dir);
                        if (neighborSite.owner != myID) {
                            isBorder = true;
                            if(neighborSite.strength < presentMap.getSite({b, a}).strength) {
                                isMoved = true;
                                moves.insert({{b, a}, dir});
                                break;
                            }
                        }
                    }
                    if(!isMoved && isBorder) {
                        isMoved = true;
                        moves.insert({{b, a}, STILL});
                    }

                    // If below certain ratio of strength/production, don't move
                    if(!isMoved && presentMap.getSite({b, a}).strength < presentMap.getSite({b, a}).production * MIN_RATIO) {
                        isMoved = true;
                        moves.insert({{b, a}, STILL});
                        continue;
                    }

                    // 1/2 chance to stay still
                    if(!isMoved && rand() % 2) {
                        isMoved = true;
                        moves.insert({{b, a}, STILL});
                        continue;
                    }

                    // Move towards border
                    if(!isMoved) {
                        moves.insert({ { b, a }, findNearestDirection(presentMap, {b, a}, myID) });
                    }
                }
            }
        }

        sendFrame(moves);   
    }

    log.close();
    return 0;
}
