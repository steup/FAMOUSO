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
#define CPU_FREQUENCY 16000000UL

#include <stdio.h>
#include <stdlib.h>
#include "mw/nl/CANNL.h"
#include "mw/nl/voidNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#ifdef __AVR__
#include "devices/nic/can/at90can/avrCANARY.h"
typedef device::nic::CAN::avrCANARY can;
#else
#include "devices/nic/can/peak/PeakCAN.h"
typedef device::nic::CAN::PeakCAN can;
#endif
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/common/UID.h"
#include "famouso.h"

#include "util/Idler.h"

#include "debug.h"

#include "mw/attributes/EmptyAttribute.h"

struct config {
    typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
    typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
    typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;
    typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
    typedef famouso::mw::el::EventLayer< anl > EL;
    typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
    typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
};
typedef config::SEC SEC;

// test the equality of types
template < typename T, typename U>
struct is_same{
    enum {value=0};
};

template < typename T>
struct is_same<T,T>{
    enum {value=1};
};

using namespace famouso::mw::attributes;

// defines a configurable Time-To-Life attribute
template<uint8_t ttl>
class TTL : public EmptyAttribute {
    uint8_t data[size];
public:
    enum {
        size = 2,
        value= ttl,
        id='T'
    };

    TTL() {
        data[0]=id;
        data[1]=value;
    }

    uint8_t get() { return data[1];}
    void set(uint8_t tmp) { data[1]=tmp;}
};

// some definition for convenience
typedef TTL<0> Local;
typedef TTL<1> Body;
typedef TTL<255> World;

// definition of an extended attribute
// the binary representation is as follows
// |C-AB|0..255 A|EP|
// C-AB -- number of attribute bytes
// A    -- the attributes in binary representation
// EP   -- event data/payload
//
// \tparam s is the size of the event payload
// \tparam Attr is an attribute attached to the event
//
template<uint16_t s=0, typename Attr=EmptyAttribute>
class ExtendedEvent : public famouso::mw::Event {
  public:
    enum {
        attr_length=Attr::size,
        local = is_same< Attr, Local >::value
    };

  private:
    // the whole event with attributes and payload
    uint8_t _edata[1+attr_length+s];

  public:
    ExtendedEvent (const famouso::mw::Subject &sub) : Event(sub) {
        _edata[0]=attr_length;
        new (&_edata[1]) Attr;
        length=s+1+attr_length;
        data=_edata;
    }

    // payload setting as simple as possible
    void operator = (const char* str) {
        uint16_t i=0;
        while( str[i] && (i<s)) {
            _edata[1+attr_length+i]=str[i];
            ++i;
        }
    }

    // try to check if the event contains the attribute <A> and
    // whether it has the right value
    template<typename A, typename comperator>
    bool check(){
        if ( (attr_length!=0) && (A::id == _edata[1]) )
            return comperator::apply(_edata[2], static_cast<uint8_t>(A::value));
        else
           return false;
    }
};


// try to find if the message contains the attribute <A>
template<typename A>
A* find(const uint8_t * const d) {
    if (d[0] && (d[1] == A::id))
        return reinterpret_cast<A*>(const_cast<uint8_t*>((&d[1])));
    else
        return reinterpret_cast<A*>(NULL);
}

// helper template for comparing to types at run-time
struct compare_equality {
    template <typename T, typename U>
    static bool apply(T x, U y) {
        return !!(x==y);
    }
};

// PublisherEventChannel definition with the ability to restrict the
// dissemination area (local network-wide)
template< typename Dissemination=EmptyAttribute>
struct P : public config::PEC {
    P() : config::PEC(famouso::mw::Subject(0xf1)){}

    template<class Ev>
    void publish(Ev e){
        if ( is_same<Dissemination, Local>::value ||
             e.template check<Local, compare_equality >() )
            ech().publish_local(e);
        else
            ech().publish(*this,e);
    }

    template<class Attr, typename Ev>
    void publish(Ev e){
        if ( is_same<Attr, Local>::value || Ev::local )
            ech().publish_local(e);
        else
            publish(e);
    }
};

// callback that is called on occurrence of an event
void cb(famouso::mw::api::SECCallBackData& cbd) {
    ::logging::log::emit() << "Michaels CallBack " << FUNCTION_SIGNATURE
            << " Parameter=" <<  cbd.length
            << " Daten:=";
    for (uint16_t i=0;i<cbd.length;++i)
        ::logging::log::emit() << cbd.data[i];
    ::logging::log::emit() << ::logging::log::endl;
    // try to find if the message contains the TTL attribute
    if ( find<TTL<0> >(cbd.data) )
        ::logging::log::emit() << "TTL enthalten" << ::logging::log::endl;
}

int main(int argc, char **argv) {

#ifdef __AVR__
    sei();
#endif

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

    // create an event with an additional attribute
    ExtendedEvent<2, TTL<1> > e(p.subject());
    e="Mi";

    // publish the event
    p.publish(e);
    p.publish<Local>(e);

    // create a second event without additional attributes
    ExtendedEvent<2> e2(p.subject());
    e2="Ma";
    p.publish<EmptyAttribute>(e2);

    Idler::idle();
    return 0;
}
