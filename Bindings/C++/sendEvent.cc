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
 * sendEvent.c
 *  - sends distance events (Remote distance Sensor)
 *
 *  Michael Schulze, 2008
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "famouso_bindings.h"

int done;

void siginthandler(int egal) {
    done = 1;
}

int main(int argc, char **argv) {
    printf("Remote distance Sensor\n");
    done = 0;
    signal(SIGINT, siginthandler);

    famouso::init<famouso::config>();

    famouso::config::PEC pec(famouso::mw::Subject(0x44697374616e6365ull));
    pec.announce();

    famouso::mw::Event e(pec.subject());
    uint8_t data[3] = {1, 50, 'v'};
    e.length = 3;
    e.data = data;

    while (!done) {
        // 25 us ist sicherlich die untere Grenze, sonst laufen die
        // internen Queues im System ueber
        // ist auch abhaengig von der Gegenseite, ob sie die Daten
        // auch abnimmt
        usleep(50000);
        // publish data
        // e.data[0]++;
        pec.publish(e);
    }
}



