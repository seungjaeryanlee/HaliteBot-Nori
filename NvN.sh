#!/bin/bash

g++ -std=c++11 Noriv7.cpp -o Nori7.o
g++ -std=c++11 Noriv6.cpp -o Nori6.o
./halite -d "30 30" "./Nori7.o" "./Nori6.o"
rm Nori7.o
rm Nori6.o