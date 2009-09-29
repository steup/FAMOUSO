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

#include <stdio.h>
#include "mw/nl/CANNL.h"
#include "mw/nl/voidNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/common/UID.h"
#include "famouso.h"

#include "util/Idler.h"
#include "debug.h"

struct config {
    typedef device::nic::CAN::PeakCAN can;
    typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
    typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
    typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;
    typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
    typedef famouso::mw::el::EventLayer< anl > EL;
    typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
    typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
};
typedef config::SEC SEC;

// callback that is called on occurrence of an event
void cb(famouso::mw::api::SECCallBackData& cbd) {
    ::logging::log::emit() << "Michaels CallBack " << FUNCTION_SIGNATURE
            << " Parameter=" <<  cbd.length
            << " Daten:=" << cbd.data << ::logging::log::endl;
}

// test on equality
template < typename T, typename U>
struct is_same{
    enum {value=0};
};

template < typename T>
struct is_same<T,T>{
    enum {value=1};
};


// attribute definitions
struct Local {};
struct ArbitraryAttribute {};


// PublisherEventChannel definition with the ability to restrict the
// dissemination area (local network-wide)
template< typename Dissemination=ArbitraryAttribute>
struct P : public config::PEC {
    P() : config::PEC(famouso::mw::Subject(0xf1)){}

    void publish(famouso::mw::Event e){
        if ( is_same<Dissemination, Local>::value)
            ech().publish_local(e);
        else
            ech().publish(*this,e);
    }

    template<class Attr>
    void publish(famouso::mw::Event e){
        if ( is_same<Attr, Local>::value)
            ech().publish_local(e);
        else
            publish(e);
    }
};

int main(int argc, char **argv) {

    // init famouso
    famouso::init<config>(argc,argv);

    // generate PublisherEventChannel without
    // dissemination restrictions
    P<> p;

    // generate a SubscriberEventChannel with the same
    // subject as the PublisherEventChannel
    SEC sec(famouso::mw::Subject(0xf1));
    sec.subscribe();
    sec.callback.bind<&cb>();

    // create an event
    famouso::mw::Event e(p.subject());
    e.length=8;
    e.data=(uint8_t*)"Michael";

    // publish the event
    p.publish(e);
    p.publish<Local>(e);
    p.publish<ArbitraryAttribute>(e);

    Idler::idle();
    return 0;
}
