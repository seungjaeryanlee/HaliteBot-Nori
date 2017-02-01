#!/bin/bash

g++ -std=c++11 Nori_5.cpp -o Nori_5.o
g++ -std=c++11 Nori_4.cpp -o Nori_4.o
./batch.py 100
rm Nori_5.o
rm Nori_4.o