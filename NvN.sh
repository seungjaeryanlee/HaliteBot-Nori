#!/bin/bash

g++ -std=c++11 Nori_5.cpp -o Nori_5.o
g++ -std=c++11 Nori_4.cpp -o Nori_4.o
./halite -d "30 30" "./Nori_5.o" "./Nori_4.o"
rm Nori_5.o
rm Nori_4.o