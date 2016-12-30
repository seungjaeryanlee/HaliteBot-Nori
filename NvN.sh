#!/bin/bash

g++ -std=c++11 Nori_1.1.cpp -o Nori_1.1.o
g++ -std=c++11 Nori_1.0.cpp -o Nori_1.0.o
./halite -d "30 30" "./Nori_1.1.o" "./Nori_1.0.o"
rm Nori_1.1.o
rm Nori_1.0.o