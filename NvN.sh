#!/bin/bash

g++ -std=c++11 Nori_3.cpp -o Nori_3.o
g++ -std=c++11 Nori_2.1.cpp -o Nori_2.1.o
./halite -d "30 30" "./Nori_3.o" "./Nori_2.1.o"
rm Nori_3.o
rm Nori_2.1.o