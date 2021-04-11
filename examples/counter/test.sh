#!/bin/bash

cd ../../
./HackDL -n COUNTER -s /Users/mbvalentin/CLionProjects/HackDL/examples/counter/tb.v \
              -l /Users/mbvalentin/CLionProjects/HackDL/examples/counter/ \
              -o /Users/mbvalentin/CLionProjects/HackDL/examples/counter/
              -p  "\bDF\d+.(Q|D)\b" \
              --monitor \
              --SEE --sim-pulses 2 --max-upset-time 10 --min-upset-time 1

# FF TMR
./HackDL -n triplicated_module -s /home/mbv6231/CLionProjects/HackDL/examples/counter/tb.v \
  -l /home/mbv6231/CLionProjects/HackDL/examples/counter/ -o /home/mbv6231/CLionProjects/HackDL/examples/counter/tmr \
  --TMR 3 -p  "\bDF\d+.(Q|D)\b"


  -n triplicated_module -s /Users/mbvalentin/CLionProjects/HackDL/examples/ECOND_ROC_DAQ/TB_ROC_DAQ_Control.sv -l /Users/mbvalentin/CLionProjects/HackDL/examples/ECOND_ROC_DAQ/ -o /Users/mbvalentin/CLionProjects/HackDL/examples/ECOND_ROC_DAQ/tmr --TMR 3 -p "\bDF\d+.(Q|D)\b"