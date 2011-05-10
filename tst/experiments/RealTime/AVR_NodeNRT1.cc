/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#define FAMOUSO_NODE_ID "SeMoNode"
#define F_CPU 16000000
#define LOGGING_DISABLE
#define NDEBUG

#define TEST_AVR_NRT
#define TEST_AVR_NO_CLOCK
#define TEST_AVR_NO_ML
#define TEST_AVR_NO_NG

#include "AVR_RTNodeCommon.h"


int main() {
    famouso::init<famouso::config>();

    using namespace famouso;

    config::PEC sensor1_pec("Sensor_1");
    sensor1_pec.announce();

    famouso::mw::Event event(sensor1_pec.subject());
    uint64_t counter = 0;

    while (1) {
        usleep(50000);

        uint8_t buffer[8];
        memset(buffer, 0, 8);
        // Tx sequence number (if MaxEventLength > 8 the rest is zeroed)
        uint64_t tmp = counter;
        for (int i = 8 - 1; i >= 0 && tmp; --i) {
            buffer[i] = tmp & 0xff;
            tmp >>= 8;
        }
        event.data = buffer;
        event.length = 8;

        sensor1_pec.publish(event);

        ++counter;
    }


    return 0;
}

