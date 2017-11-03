# NetSim
Distributed Memory Event-Driven Simulation of an Integrate and Fire Neural Network using Open MPI.

Netbeans project built on Ubuntu 16.04

Program can be run by simple execution:

>> ./netsim_parallel_event

Which will simulate an Erdős–Rényi type of neural network with 120 neurons and connection probability = 0.2 for 10 seconds with a timestep of 0.1 ms.

The desired parameters can be used for the simulation by running:

>> ./netsim_parallel_event [Number of Neurons] [Connection Probability] [Number of Timesteps to Simulate]

