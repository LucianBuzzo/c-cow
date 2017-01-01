# /bin/bash

mkdir -p bin
rm bin/*
g++ -std=c++11 src/c-cow.c -o bin/c-cow
