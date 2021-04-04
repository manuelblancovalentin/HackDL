#!/bin/bash

cd ../../
./HackDL -n COUNTER -s /Users/mbvalentin/CLionProjects/HackDL/examples/counter/tb.v \
              -l /Users/mbvalentin/CLionProjects/HackDL/examples/counter/ \
              -o /Users/mbvalentin/CLionProjects/HackDL/examples/counter/
              -p  "\bDF\d+.(Q|D)\b" \
              --monitor \
              --SEE --sim-pulses 2 --max-upset-time 10 --min-upset-time 1