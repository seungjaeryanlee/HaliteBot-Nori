#!/bin/bash

g++ -std=c++11 NoriBot.cpp -o NoriBot.o
g++ -std=c++11 RandomBot.cpp -o RandomBot.o
./halite -d "30 30" "./NoriBot.o" "./RandomBot.o"
