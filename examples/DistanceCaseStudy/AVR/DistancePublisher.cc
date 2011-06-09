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
#include "gp2d120.h"

uint8_t adc_get_value(uint8_t channel) {
    ADCSRA|=(1<<ADEN) | 0x7;
    ADMUX=channel | (1<<ADLAR);
    ADMUX|=((1<<REFS0)|(1<<REFS1));
    ADCSRA|=(1<<ADSC);
    while(ADCSRA & (1<<ADSC));
	return ADCH;
}

int main() {
    sei();
    ::logging::log::emit()
        << PROGMEMSTRING("Starting CAN example publisher!")
        << ::logging::log::endl << ::logging::log::endl;

    famouso::init<famouso::config>();

    famouso::config::PEC pec(distance);
    pec.announce();

    famouso::mw::Event e(pec.subject());
    uint8_t data[2]={0,0};
    e.length = 2;
    e.data   = data;

    do {
        uint8_t adc_value=adc_get_value(2);
		data[1]=gp2d120_adc8_to_cm(adc_value);
        pec.publish(e);
        delay_ms(50);
    } while (true);
}
