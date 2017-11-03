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
 * File:   Neuron.cpp
 * Author: chi
 * 
 * Created on May 8, 2017, 12:22 PM
 */

#include "Neuron.h"
#include "Subnetwork.h"

#ifdef DEBUG
int Neuron::counter = 0;
#endif

Neuron::Neuron(const double &membrane_potential)
: MembranePotential(membrane_potential), Time(0) {}

Neuron::~Neuron() {}

bool Neuron::Integrate(const int &time, const double &amplitude) {
    double k1, k2, k3, k4;
    while(Time < time) {
        k1 = DifferentialEquation(MembranePotential);
        k2 = DifferentialEquation(MembranePotential + DT * k1 * 0.5);
        k3 = DifferentialEquation(MembranePotential + DT * k2 * 0.5);
        k4 = DifferentialEquation(MembranePotential + DT * k3);
        MembranePotential += 0.5 * DT * (k1 + k2 + k3 + k4); 
        ++Time;
    }
    MembranePotential += amplitude;
    if(MembranePotential > FIRING_POTENTIAL) {
#ifdef DEBUG
        ++counter;
#endif
        MembranePotential = RESET_POTENTIAL;
        return 1;
    }
    return 0;
}

inline double Neuron::DifferentialEquation(double potential) const {
    return (RESTING_POTENTIAL - potential + SHARPNESS_DT * \
            exp((potential - FIRING_POTENTIAL) / SHARPNESS_DT)) / TIME_CONSTANT;
}

bool Order (const Event* first, const Event* second) {
    return (first->time < second->time);
}