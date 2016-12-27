#!/bin/bash

g++ -std=c++11 Noriv6.cpp -o Nori6.o
g++ -std=c++11 Noriv5.cpp -o Nori5.o
./halite -d "30 30" "./Nori6.o" "./Nori5.o"
rm Nori6.o
rm Nori5.o