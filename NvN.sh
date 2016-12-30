#!/bin/bash

g++ -std=c++11 Nori_2.0.cpp -o Nori_2.0.o
g++ -std=c++11 Nori_1.2.cpp -o Nori_1.2.o
./halite -d "30 30" "./Nori_2.0.o" "./Nori_1.2.o"
rm Nori_2.0.o
rm Nori_1.2.o