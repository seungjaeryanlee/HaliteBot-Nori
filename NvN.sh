#!/bin/bash

g++ -std=c++11 NoriBotv4.cpp -o Nori4.o
g++ -std=c++11 NoriBotv3.cpp -o Nori3.o
./halite -d "30 30" "./Nori4.o" "./Nori3.o"
rm Nori4.o
rm Nori3.o