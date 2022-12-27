#!/bin/bash

echo "Enter number of process:"

read processNumber

if [[ -z "$processNumber" || "$processNumber" -lt 1 ]]; then
	echo "Unavalible number of process"
    else
	mpic++ star.cpp -o star.exe
	mpirun -np $processNumber ./star.exe
fi
