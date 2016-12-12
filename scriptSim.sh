#!/bin/bash
for i in 10 100 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000
do
   echo "Simulation of $i buildings and 5 Appliances is running (Ack). Output is in /logs/CaseA/"$i"-5-Ack.log".
   { time ./SGSim $i 5 1;} &> ./logs/CaseA/"$i-5-Ack".log
   echo "Simulation of $i buildings and 5 Appliances is running (NoAck). Output is in /logs/CaseA/"$i"-5-NoAck.log".
   { time ./SGSim $i 5 0;} &> ./logs/CaseA/"$i-5-NoAck".log
   echo "Done"
done

for j in  1 2 4 6 8 10 12 14 16 18 20
do
   echo "Simulation of 500 buildings and $j Appliances is running. Output is in /logs/CaseB/500-"$j"-Ack.log".
   { time ./SGSim 500 $j 1;} &> ./logs/CaseB/"500-$j-Ack".log
   echo "Done"
done

