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
 * File:   Subnetwork.cpp
 * Author: chi
 * 
 * Created on May 25, 2017, 8:26 PM
 */

#include "Subnetwork.h"

Subnetwork::Subnetwork(const unsigned int& nr_neurons, const unsigned int& connection_proba, \
        const unsigned int& nr_timesteps, int rank, int cluster_size) \
        : Duration(nr_timesteps), Rank(rank), ClusterSize(cluster_size){
    if(cluster_size) NrNeurons = nr_neurons / cluster_size;
    else NrNeurons = nr_neurons;
    srand(SEED+pow(rank,3));
    InitializeSynapses(connection_proba, InitializeNeurons());
    InitializeInput();    
}

Subnetwork::~Subnetwork() {
    
    for(unsigned int n = 0; n < NrNeurons; ++n) {
        delete Neurons[n];
    }
    
    for(Event* event: Events){
        delete event;
    }
    
    for(std::pair<int,std::list<Synapse*> > synapses: Synapses){
        for(Synapse* synapse: synapses.second) {
            delete synapse;
        }
    }
    
    delete[] Neurons;
}

double* Subnetwork::InitializeNeurons() {
    double membrane_potential;
    int index = 0;
    Neurons = new Neuron*[NrNeurons];
    double* position = new double[NrNeurons*3];
    for(unsigned int n=0; n < NrNeurons; ++n) {
        membrane_potential = RESTING_POTENTIAL + (rand() % (int)(FIRING_POTENTIAL-RESTING_POTENTIAL)); 
        position[index] = rand() % HEIGHT;
        position[index+1] = rand() % WIDTH;
        position[index+2] = rand() % DEPTH;
        Neurons[n] = new Neuron(membrane_potential);
        index += 3;
    }
    return position;
}

void Subnetwork::InitializeSynapses(const double& connection_proba, double* postpos) {
    
    double* prepos = new double[ClusterSize*NrNeurons*3];
    double weight;
    double distance;
    int index;
    
    MPI_Allgather(postpos, 3*NrNeurons, MPI_DOUBLE, prepos, 3*NrNeurons, \
            MPI_DOUBLE, MPI_COMM_WORLD);
    
    for(unsigned int presyn = 0; presyn < NrNeurons*ClusterSize; ++presyn) {
        index = 3*presyn; 
        std::list<Synapse*> synapses;
        for(unsigned int postsyn = 0; postsyn < NrNeurons; ++postsyn) {
            if(rand() % 100 < connection_proba) {
                weight = rand() % (int)(MAX_AP - MIN_AP);
                distance = 0;
                for(int pos = 0; pos < 3; ++pos) {
                    distance += pow(prepos[index+pos] - postpos[3*postsyn+pos], 2);
                }
                distance = sqrt(distance);
                synapses.push_back(new Synapse(postsyn, weight, \
                        MIN_DELAY + (int)(distance / AP_SPEED)));
            }
        }
        synapses.sort([](const Synapse* a, const Synapse* b) { return a->delay < b->delay; });
        Synapses.insert(std::make_pair(presyn,synapses));
    }    
    delete[] prepos;
    delete[] postpos;
}

void Subnetwork::InitializeInput() {
    double frequency[NrNeurons];
    double amplitude[NrNeurons];   
    for(unsigned int n = 0; n < NrNeurons; ++n) {
        frequency[n] = MIN_INPUT_FREQ + (rand()%(MAX_INPUT_FREQ-MIN_INPUT_FREQ)); 
        amplitude[n] = rand() % (int)(MAX_AP - MIN_AP);
    }
    for(unsigned int t=0; t < Duration; ++t) {
        for(unsigned int n = 0; n < NrNeurons; ++n) {
            if((rand() % (int)(1000/DT)) < frequency[n]) {
                Events.push_back(new Event(n, amplitude[n], t));
            }
        }
    }
    for(unsigned int n = 0; n < NrNeurons; ++n) {
        Events.push_back(new Event(n, 0, Duration));
    }
}

void Subnetwork::Simulate() {
    unsigned int eventtime, index;
    const int nr1 = NrNeurons+1;
    unsigned int common_clock = MIN_DELAY;
    int* events = new int[nr1]();
    int* recv_events = new int[ClusterSize*nr1];
    Event* current_event;
    std::list<Event*> event_buffer;
    std::list<Event*> new_events;

    while(!Events.empty()) {
        current_event = Events.front();
        if(current_event->time <= common_clock && !events[current_event->neuron]){
            Events.pop_front();
            if(Neurons[current_event->neuron]->Integrate(current_event->time, current_event->weight)) {
                events[current_event->neuron] = current_event->time;
            }
            delete current_event;
        }
        else{
            events[NrNeurons] = current_event->time;
            MPI_Allgather(events, nr1, MPI_INT, recv_events, nr1, \
                    MPI_INT, MPI_COMM_WORLD);
            common_clock = current_event->time;

            for(unsigned int node=0; node < ClusterSize; ++node){
                index = node*nr1;
                for(unsigned int presyn=0; presyn <  NrNeurons; ++presyn){
                    if(recv_events[index+presyn]){
                        for(Synapse* synapse: Synapses[node*NrNeurons+presyn]) {
                            eventtime = recv_events[index+presyn] + synapse->delay;
                            if(eventtime < Duration) event_buffer.push_back(\
                                    new Event(synapse->postneuron, \
                                    synapse->weight, eventtime));
                        }
                        new_events.merge(event_buffer, Order);
                        event_buffer.clear();
                    }
                }
                if((int)common_clock > recv_events[index+NrNeurons]){
                    common_clock = recv_events[index+NrNeurons];
                }
            }
            if(!new_events.empty() && common_clock>new_events.front()->time) \
                    common_clock = new_events.front()->time;
            if(!common_clock) common_clock = Duration;
            common_clock += MIN_DELAY;
            Events.merge(new_events, Order);
            new_events.clear();
            delete[] events;
            events = new int[nr1]();
            
        }
    }
    while(index) {
        index = 0;
        for(unsigned int node=0; node < ClusterSize; ++node){
            if(recv_events[node*nr1 + NrNeurons]) {
                index = 1;
                MPI_Allgather(events, nr1, MPI_INT, recv_events, nr1, \
                        MPI_INT, MPI_COMM_WORLD);
                break;
            }
        }
    }
    delete[] events;
    delete[] recv_events;
}

#ifdef DEBUG
void Subnetwork::Readout() const {
    std::ofstream logfile;
    logfile.open ("log"+std::to_string(Rank)+".txt");
    std::string string_potential;
    for(unsigned int neuron=0; neuron <  NrNeurons; ++neuron){
        std::ostringstream strs;
        strs << Neurons[neuron]->GetMembranePotential();
        string_potential = strs.str();
        logfile << "Node: " << Rank << " Neuron: " << neuron << " Potential: " \
                << string_potential << std::endl;
    }
    logfile << "Total count of AP for node: " << Rank << " was " \
            << Neurons[0]->counter << std::endl;
    logfile.close();
}
#endif