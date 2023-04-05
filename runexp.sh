#!/bin/bash

make -j4                   # makes the build using 4 threads

rm data.dat                # removes the "data.dat" file if it exists
rm efficiency_data.dat     # removes the "efficiency_data.dat" file if it exists

./exampleB4c -m run1.mac   # runs the exampleB4c executable using run1.mac
