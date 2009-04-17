/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#include <stddef.h>
#include <stdint.h>

#include "mw/nl/CANNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "devices/nic/can/at90can/avrCANARY.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/common/UID.h"
#include "famouso.h"

typedef device::nic::CAN::avrCANARY can;
typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;

typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
typedef famouso::mw::el::EventLayer< anl > el;
typedef famouso::mw::api::EventChannel< el > EC;
typedef famouso::mw::api::PublisherEventChannel<el> PEC;
typedef famouso::mw::api::SubscriberEventChannel<el> SEC;


#ifndef AVR
#define DEBUG(x) printf x
void done() {}
void init() {}
#else
inline void init() {}

include < avr / io.h >

inline void usart1_transmit(unsigned char data) {
    /* Wait for empty transmit buffer */
    while (!(UCSR1A & (1 << UDRE0)));
    /* Start transmittion */
    UDR1 = data;
}

/* Initialize UART */
inline void init() {
    /* Set the baud rate */
    UBRR1H = (unsigned char)(51 >> 8);
    UBRR1L = (unsigned char) 51;

    /* Enable UART receiver and transmitter */
    UCSR1B = ((1 << RXEN1) | (1 << TXEN1));

    /* Set frame format: 8N1 */
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

inline void usart1_transmit_string(char* p) {
    while (*p)
        usart1_transmit(*p++);
}

#define DEBUG(X) usart1_transmit_string X
inline void done() {
    while (1);
}
#endif

void cb(famouso::mw::api::SECCallBackData& cbd) {
    DEBUG(("\r\nMichaels CallBack\r\n\r\n"));
}

int  main() {
    init();
    // greet the world
    DEBUG(("Hello World\r\n"));
    sei();

    famouso::init<EC>();
    SEC sec(0xf1);
    sec.subscribe();
    sec.callback.bind<&cb>();

    PEC pec(0xf1);
    pec.announce();

    famouso::mw::Event e(pec.subject());
    uint8_t data[3] = {1, 50, 'v'};
    e.length = 3;
    e.data = data;

    pec.publish(e);
    DEBUG(("publishing done\r\n"));

    done();
}
