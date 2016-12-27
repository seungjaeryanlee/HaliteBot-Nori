#!/bin/bash

g++ -std=c++11 NoriBotv3.cpp -o NoriBotv3.o
g++ -std=c++11 NoriBotv2.cpp -o NoriBotv2.o
./halite -d "30 30" "./NoriBotv3.o" "./NoriBotv2.o"
rm NoriBotv3.o
rm NoriBotv2.o