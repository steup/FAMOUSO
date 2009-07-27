/*******************************************************************************
 *
 * Copyright (c) 2008, 2009
 * Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
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
/*! \brief  Example program to demonstrate.  */
#ifdef __AVR_AT90CAN128__
#define CPU_FREQUENCY 16000000UL
#else
#define CPU_FREQUENCY 8000000UL
#endif
#define F_CPU CPU_FREQUENCY
#define USE_PHY_CLASS

#include <util/delay.h>

#include "avr-halib/avr/regmaps.h"
#include "avr-halib/avr/uart.h"

#include "avr-halib/share/cdevice.h"
#include "avr-halib/share/cbuffer.h"

//-----------------------------------------------------------------------------
UseInterrupt(SIG_UART1_RECV);
UseInterrupt(SIG_UART1_DATA);
CDevice< SecOut< CInBuffer< COutBuffer< Uart < Uart1<> > ,uint8_t,200> ,uint8_t,200> >  >uart;

#include "config.h"

void cb( famouso::mw::api::SECCallBackData& cbd) {
    uart << " [ SEC-CB(" << cbd.length << "): \"" << (char*)cbd.data << "\"  ]\n\r";
}
//-----------------------------------------------------------------------------
int main() {
    sei();                              // enable interrupts

    uart << "Starting demonstation of IEEE 802.15.4 communication (S)!\n";
    uart.writeNewline();
    //-------------------------------------------------------------------------
    famouso::init<famouso::config>();   // initialize famouso

    famouso::config::SEC sec(subject_d);
    sec.subscribe();
    sec.callback.bind<&cb>();
    //-------------------------------------------------------------------------
    do {                                // duty cycle
        uart << ".";
        for (int i = 0;i < 100;i++) _delay_ms(10);
    } while(true);
    //-------------------------------------------------------------------------
    return (1);
}
