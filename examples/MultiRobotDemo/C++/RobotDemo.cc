/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/

/*
 * RobotDemo.cc
 *
 *
 *  Michael Schulze, 2008
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "famouso_bindings.h"
#include "util/endianness.h"

#define RobotID 4

int done;
char HumanDetectionSubject[] = "HumanDet";
char Distance[] = "Distance";
char Velocity[] = "Velocity";

volatile uint8_t DistanceSensorValue = 200;

void SensorValueCB(famouso::mw::api::SECCallBackData& cbd) {
    if ((cbd.data[2] == RobotID)) {
        DistanceSensorValue = cbd.data[0];
    }
}

volatile uint8_t Human = 0;
void HumanDetectedCB(famouso::mw::api::SECCallBackData& cbd) {
    Human = cbd.data[0];
};

void siginthandler(int egal) {
    done = 1;
}

int main(int argc, char **argv) {
    done = 0;
    signal(SIGINT, siginthandler);

    ::logging::log::emit() << "FAMOUSO -- Initialization started." << ::logging::log::endl;
    famouso::init<famouso::config>();
    ::logging::log::emit() << "FAMOUSO -- Initialization successful." << ::logging::log::endl;

    famouso::config::SEC sec(Distance);
    sec.subscribe();
    sec.callback.bind<SensorValueCB>();

    famouso::config::SEC H(HumanDetectionSubject);
    H.subscribe();
    H.callback.bind<HumanDetectedCB>();

    famouso::config::PEC pec(Velocity);
    famouso::mw::Event e(pec.subject());
    uint8_t data[3] = {120, 120, RobotID};
    e.length = 3;
    e.data = data;
    pec.announce();

    while (!done) {
        if (Human != 0) {
            // drive(STOP);
            data[0] = 0;
            data[1] = 0;
        } else {
            if (DistanceSensorValue < 50) {
                // drive(LEFT);
                data[0] = 20;
                data[1] = -20;
            } else {
                // drive(STRAIGHT);
                data[0] = 120;
                data[1] = 120;
            }
        }
        pec.publish(e);
        // mam muss nicht unbedingt warten, jedoch entlasstet dies
        // die cpu, weil es sonst busy-waiting ist
        usleep(50000);
    }
}
