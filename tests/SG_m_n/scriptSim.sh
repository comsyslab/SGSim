#!/bin/bash
for i in 10 100 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000
do
   echo "Simulation of $i buildings and 5 Appliances is running. Output is in "$i"-5.log".
   { time ./SG_m_n $i 5;} &> ./logs/"$i-5".log
   echo "Done"
done
