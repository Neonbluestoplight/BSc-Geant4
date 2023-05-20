#!/bin/bash

make -j4                          # makes the build using 4 threads

rm B4.root

rm data.dat                       # removes the "data.dat" file if it exists
rm diode_efficiency_data.dat      # removes the "diode_efficiency_data.dat" file if it exists
rm annular_efficiency_data.dat    # removes the "annular_efficiency_data.dat" file if it exists
rm collective_efficiency_data.dat # removes the "collective_efficiency_data.dat" file if it exists

./exampleB4c -m run1.mac          # runs the exampleB4c executable using run1.mac
