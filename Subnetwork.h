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
 * File:   Subnetwork.h
 * Author: chi
 *
 * Created on May 25, 2017, 8:26 PM
 */

#ifndef SUBNETWORK_H
#define SUBNETWORK_H

#include "Neuron.h"
#include <iostream>
#include <fstream>
#include <sstream>

const unsigned int HEIGHT = 100; // mm
const unsigned int WIDTH = 5; // mm
const unsigned int DEPTH = 5; // mm
const unsigned int SEED = 11483952;
const unsigned int MAX_INPUT_FREQ = 1000; // Hz/INPUT_FREQ_FACTOR
const unsigned int MIN_INPUT_FREQ = 0; // Hz/INPUT_FREQ_FACTOR

class Subnetwork {
public:
    Subnetwork(const unsigned int& nr_neurons, const unsigned int& connection_proba, \
            const unsigned int& nr_timesteps, int rank, int cluster_size);
    virtual ~Subnetwork();
    void Simulate();
#ifdef DEBUG
    void Readout() const;
#endif
private:
    inline double* InitializeNeurons();
    inline void InitializeSynapses(const double& connection_proba, \
            double* postpos);
    inline void InitializeInput();
    Neuron** Neurons;
    synapsemap Synapses;
    std::list<Event*> Events;
    unsigned int Duration;
    unsigned int NrNeurons;
    int Rank;
    unsigned int ClusterSize;

};

#endif /* SUBNETWORK_H */

