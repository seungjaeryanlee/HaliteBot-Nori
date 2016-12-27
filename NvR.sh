#!/bin/bash

g++ -std=c++11 NoriBotv3.cpp -o NoriBotv3.o
g++ -std=c++11 RandomBot.cpp -o RandomBot.o
./halite -d "30 30" "./NoriBotv3.o" "./RandomBot.o"
rm NoriBotv3.o
rm RandomBot.o