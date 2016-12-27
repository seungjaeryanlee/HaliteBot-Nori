#!/bin/bash

g++ -std=c++11 Noriv5.cpp -o Nori5.o
g++ -std=c++11 Noriv4.cpp -o Nori4.o
./halite -d "30 30" "./Nori5.o" "./Nori4.o"
rm Nori5.o
rm Nori4.o