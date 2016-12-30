#!/bin/bash

g++ -std=c++11 Nori_1.0.cpp -o Nori_1.0.o
g++ -std=c++11 Nori_0.6.cpp -o Nori_0.6.o
./halite -d "30 30" "./Nori_1.0.o" "./Nori_0.6.o"
rm Nori_1.0.o
rm Nori_0.6.o