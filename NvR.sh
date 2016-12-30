#!/bin/bash

g++ -std=c++11 Nori_1.0.cpp -o Nori_1.0.o
g++ -std=c++11 RandomBot.cpp -o RandomBot.o
./halite -d "30 30" "./Nori_1.0.o" "./RandomBot.o"
rm Nori_1.0.o
rm RandomBot.o