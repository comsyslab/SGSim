# SmartGridSimulator
SCNSL-based simulator for smart grid Server&lt;->Building&lt;->Appliances hierarchy

# To run the simulator
1. Install SystemC Network Simulation library `https://sourceforge.net/projects/scnsl/` .
2. replace tests folder with the one in the repository.
3. compile and execute SG_m_n example.
4. pass the number of buildings, appliances and re-transmission options (0/1) as arguments to ./SG\_m\_n. e.g.,  ./SG\_m\_n 10 5 1 (means 10 buildings, 5 appliances, with Ack).
5. run it.
6. there is a bash script that execute a sequence of building and appliances with the Ack. The script is called `scriptSim.sh` 
7. to run the script in the background and close the SSH, run `tmux` and execute the script and then click `CTRL+B` and `D`.
