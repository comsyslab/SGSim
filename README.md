# SmartGridSimulator
SCNSL-based simulator for smart grid Server&lt;->Building&lt;->Appliances hierarchy

# To run the simulator
1. Install SystemC Network Simulation library `https://sourceforge.net/projects/scnsl/` .
2. Download SGSim and add it inside scnsl_beta2 directory.
3. mkdir obj; cd obj; cmake ..; make; 
4. pass the number of buildings, appliances and re-transmission options (0/1) as arguments to ./SGSim. e.g.,  ./SGSim 10 5 1 (means 10 buildings, 5 appliances, with Ack).
5. run it.
6. There is a bash script that execute a sequence of building and appliances with the Ack. The script is called `scriptSim.sh` 
7. To run the script in the background and close the SSH, run `tmux` and execute the script and then click `CTRL+B` and `D`.
8. To scale up the simulation, setup the memory mapping `$sudo bash -c 'echo 131060 > /proc/sys/vm/max_map_count'`
