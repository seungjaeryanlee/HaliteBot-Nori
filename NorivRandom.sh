#!/bin/bash

g++ -std=c++11 NoriBotv2.cpp -o NoriBotv2.o
g++ -std=c++11 RandomBot.cpp -o RandomBot.o
./halite -d "30 30" "./NoriBotv2.o" "./RandomBot.o"
rm NoriBotv2.o
rm RandomBot.o