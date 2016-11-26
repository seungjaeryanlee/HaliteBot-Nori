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
     sendInit("SkewedRandomC++Bot");

     std::set<hlt::Move> moves;
     while (true) {
          moves.clear();

          getFrame(presentMap);

          for (unsigned short a = 0; a < presentMap.height; a++) {
               for (unsigned short b = 0; b < presentMap.width; b++) {
                    if (presentMap.getSite({ b, a }).owner == myID) {
                         switch (rand() % 8) {
                         case 0: case 1: case 2: case 3: 
                              moves.insert({ {b, a}, (unsigned char)STILL });
                              break;
                         case 4:
                              moves.insert({ { b, a }, (unsigned char)NORTH });
                              break;
                         case 5:
                              moves.insert({ { b, a }, (unsigned char)SOUTH });
                              break;
                         case 6:
                              moves.insert({ { b, a }, (unsigned char)EAST });
                              break;
                         case 7:
                              moves.insert({ { b, a }, (unsigned char)WEST });
                              break;
                         }
                    }
               }
          }

          sendFrame(moves);
     }

     return 0;
}
