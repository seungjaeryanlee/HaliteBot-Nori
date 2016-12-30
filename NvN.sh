#!/bin/bash

g++ -std=c++11 Nori_1.2.cpp -o Nori_1.2.o
g++ -std=c++11 Nori_1.1.cpp -o Nori_1.1.o
./halite -d "30 30" "./Nori_1.2.o" "./Nori_1.1.o"
rm Nori_1.2.o
rm Nori_1.1.o