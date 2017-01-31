#!/bin/bash

g++ -std=c++11 Nori_4.cpp -o Nori_4.o
g++ -std=c++11 Nori_3.cpp -o Nori_3.o
./batch.py 100
rm Nori_4.o
rm Nori_3.o