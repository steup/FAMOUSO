/*******************************************************************************
 *
 * Copyright (c) 2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#include "famouso-cfg.h"

void LedMeter(unsigned char v){
	DDRF=255;
	if (v>4) PORTF|=1; else PORTF&=~1;
	if (v>8) PORTF|=2; else PORTF&=~2;
	if (v>12) PORTF|=4; else PORTF&=~4;
	if (v>16) PORTF|=8; else PORTF&=~8;
	if (v>19) PORTF|=16; else PORTF&=~16;
	if (v>22) PORTF|=32; else PORTF&=~32;
	if (v>25) PORTF|=64; else PORTF&=~64;
	if (v>28) PORTF|=128; else PORTF&=~128;
}

void handleCallback( famouso::mw::api::SECCallBackData& cbd) {
        if (cbd.data[0]==0) LedMeter(cbd.data[1]);
}

int main() {
    // enable interrupts
    sei();

    ::logging::log::emit()
        << PROGMEMSTRING("Starting CAN example subscriber!")
        << ::logging::log::endl << ::logging::log::endl;

    famouso::init<famouso::config>();

    // create a subscriber channel, subscribe to it and
    // bind a callback that is called upon event reception
    famouso::config::SEC sec(distance);
    sec.subscribe();
    sec.callback.bind<handleCallback>();

    Idler::idle();
}
