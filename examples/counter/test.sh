#!/bin/bash

cd ../../cmake-build-debug/
echo "Performing Full-TMR on counter module..."
./HackDL -n COUNTER -s /Users/mbvalentin/CLionProjects/HackDL/examples/counter/counter.v \
              -d /Users/mbvalentin/CLionProjects/HackDL/examples/counter/ \
              -o /Users/mbvalentin/CLionProjects/HackDL/examples/counter/tmrfull \
              -p  "\bC\d+$" \
              --TMR full
#--monitor \
#--SEE --sim-pulses 2 --max-upset-time 10 --min-upset-time 1

echo "Performing logic triplication on counter module..."
./HackDL -n COUNTER -s /Users/mbvalentin/CLionProjects/HackDL/examples/counter/counter.v \
              -d /Users/mbvalentin/CLionProjects/HackDL/examples/counter/ \
              -o /Users/mbvalentin/CLionProjects/HackDL/examples/counter/tmrlogic \
              -p  "\bC\d+$" \
              --TMR logic

echo "Performing clk skew triplication on counter module..."
./HackDL -n COUNTER -s /Users/mbvalentin/CLionProjects/HackDL/examples/counter/counter.v \
              -d /Users/mbvalentin/CLionProjects/HackDL/examples/counter/ \
              -o /Users/mbvalentin/CLionProjects/HackDL/examples/counter/tmrclk \
              -p  "\bC\d+$" \
              --TMR clk

echo "Performing output triplication on counter module..."
./HackDL -n COUNTER -s /Users/mbvalentin/CLionProjects/HackDL/examples/counter/counter.v \
              -d /Users/mbvalentin/CLionProjects/HackDL/examples/counter/ \
              -o /Users/mbvalentin/CLionProjects/HackDL/examples/counter/tmrout \
              -p  "\bC\d+$" \
              --TMR out

echo "Performing input triplication on counter module..."
./HackDL -n COUNTER -s /Users/mbvalentin/CLionProjects/HackDL/examples/counter/counter.v \
              -d /Users/mbvalentin/CLionProjects/HackDL/examples/counter/ \
              -o /Users/mbvalentin/CLionProjects/HackDL/examples/counter/tmrin \
              -p  "\bC\d+$" \
              --TMR in