#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>

#include "hlt.hpp"
#include "networking.hpp"

int main() {
    srand(time(NULL));

    std::cout.sync_with_stdio(0);

    unsigned char myID;
    hlt::GameMap presentMap;
    getInit(myID, presentMap);
    sendInit("NoriBot");

    std::set<hlt::Move> moves;
    while(true) {
        moves.clear();

        getFrame(presentMap);

        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if (presentMap.getSite({ b, a }).owner == myID) {
                    // If at zero strength, don't move
                    if(presentMap.getSite({ b, a}).strength == 0) {
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

    return 0;
}
