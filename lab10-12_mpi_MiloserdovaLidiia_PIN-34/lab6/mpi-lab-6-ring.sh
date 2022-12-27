#!/bin/bash

echo "Enter number of process:"

read processNumber

if [[ -z "$processNumber" || "$processNumber" -lt 1 ]]; then
	echo "Unavalible number of process"
    else
	mpic++ ring.cpp -o ring.exe
	mpirun -np $processNumber ./ring.exe
fi
