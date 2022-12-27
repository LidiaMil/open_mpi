#!/bin/bash

echo "Number of process: 8"

mpic++ main.cpp -o main.exe
mpirun -np 8 ./main.exe
