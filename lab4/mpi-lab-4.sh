#!/bin/bash

echo "Enter number of process:"

read processNumber

if [[ -z "$processNumber" || "$processNumber" -lt 1 ]]; then
	echo "Unavalible number of process"
    else
	mpic++ main.cpp -o main.exe
	mpirun -np $processNumber ./main.exe
fi
