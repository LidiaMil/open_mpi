#!/bin/sh

poetry run mpiexec -n $1 --use-hwthread-cpus --oversubscribe python counter-3.py