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
 * File:   Neuron.h
 * Author: chi
 *
 * Created on May 8, 2017, 12:22 PM
 */

#ifndef NEURON_H
#define NEURON_H

#include <stdlib.h>
#include <list>
#include <math.h>
#include <mpi.h>
#include <map>


const double RESTING_POTENTIAL = -60; // mV
const double RESET_POTENTIAL = -65; // mV
const double FIRING_POTENTIAL = -30; // mV
const double TIME_CONSTANT = 12; // ms
const unsigned int SHARPNESS_DT  = 2; // mV
const double DT = 0.1; // ms per timestep
const int MIN_DELAY = 30; // timesteps
const double AP_SPEED = 1; // mm/ms = m/s
const double MIN_AP = 10; // mV
const double MAX_AP = 40; // mV

class Neuron;
class Subnetwork;

struct Event{
    Event(int n, double w, unsigned int t): neuron(n), weight(w), time(t){}
    int neuron;
    double weight;
    unsigned int time;
};

struct Message{
    Event event;
    int node;
};

struct Synapse{
    Synapse(int postneu, double w, int d)
            : postneuron(postneu), weight(w), delay(d) {}
    int postneuron;
    double weight;
    int delay;
};

typedef std::map<int,std::list<Synapse*> > synapsemap;

class Neuron {
public:
    Neuron(const double &membrane_potential);
    virtual ~Neuron();
    inline double GetMembranePotential() const { return MembranePotential; };
    bool Integrate(const int &time, const double &amplitude);
#ifdef DEBUG
    static int counter;
#endif
private:
    inline double DifferentialEquation(double potential) const;
    double MembranePotential;
    int Time;
    
};

bool Order (const Event* first, const Event* second);

#endif /* NEURON_H */

