/*
* Copyright (c) 2017, Matthias Chinyen Tsai
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. All advertising materials mentioning features or use of this software
*    must display the following acknowledgement:
*    This product includes software developed by Matthias C. Tsai.
* 4. The name of Matthias C. Tsai may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY MATTHIAS CHINYEN TSAI ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL MATTHIAS CHINYEN TSAI BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* 
 * File:   main.cpp
 * Author: chi
 *
 * Created on May 8, 2017, 12:54 PM
 */

#include "Subnetwork.h"
#include <ctime>


using namespace std;

/*
 * Main program of the sequential, event-driven neuronal network simulation
 * program Netsim
 */
int main(int argc, char** argv) {
    
    
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int cluster_size;
    MPI_Comm_size(MPI_COMM_WORLD, &cluster_size);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    
    // Initialize parameters
    unsigned int nr_neurons;
    double connection_proba;
    unsigned int nr_timesteps;
    if(argc == 4) {
        nr_neurons = strtol(argv[1], NULL, 10);
        connection_proba = atof(argv[2]);
        nr_timesteps = strtol(argv[3], NULL, 10);
    } 
    else {
        if(argc == 2) nr_neurons = strtol(argv[1], NULL, 10);
        else nr_neurons = 120;
        connection_proba = 0.2;
        nr_timesteps = 100000;
    }
    
    if(nr_neurons % cluster_size != 0) {
        if(rank == 0) {
            std::cout << "Number of neurons in the network should be "
                    "divisible by the number of nodes used" << std::endl;
        }
        return 0;
    }

    printf("Build Network %i\n",rank);
    clock_t begin = clock();
    Subnetwork network(nr_neurons, connection_proba, nr_timesteps, rank, cluster_size);
    if(rank == 0) printf("Run Simulation...\n");
    if(rank == 0) printf("network memory %lu\n",sizeof(Synapse));
    clock_t middle = clock();
    network.Simulate();
    MPI_Barrier(MPI_COMM_WORLD);
    clock_t end = clock();
    double netbuildt = double(middle - begin) / CLOCKS_PER_SEC;
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    if(rank == 0) printf("Simulation over after %f seconds for the entire "
            "simulation and %f seconds for building the network\n", \
            elapsed_secs, netbuildt);
    
#ifdef DEBUG
    network.Readout();
    if(rank == 0) printf("Simulation logged\n");
#endif
    
    // Finalize the MPI environment.
    MPI_Finalize();
    if(rank == 0) printf("Netsim terminated successfully with all %i nodes\n", \
            cluster_size);
    return 0;
}