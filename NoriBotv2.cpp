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
    sendInit("NoriBot");

    std::set<hlt::Move> moves;
    for(int i = 1; ; i++) {
        moves.clear();

        log << "Turn " << i << ": " << findStrength(presentMap, myID) << " strength" << std::endl;

        getFrame(presentMap);

        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID) {
                    // If below ratio of strength/production, don't move
                    if(presentMap.getSite({b, a}).strength < presentMap.getSite({b, a}).production * MIN_RATIO) {
                        moves.insert({{b, a}, STILL});
                        continue;
                    }

                    // 1/2 chance to stay still, 1/8 chance to move each direction
                    if(rand() % 2) {
                        moves.insert({{b, a}, STILL});
                        continue;
                    }
                    else {
                        moves.insert({ { b, a }, (unsigned char)(rand() % 4 + 1) });
                        continue;
                    }
                }
            }
        }

        sendFrame(moves);   
    }

    log.close();
    return 0;
}
