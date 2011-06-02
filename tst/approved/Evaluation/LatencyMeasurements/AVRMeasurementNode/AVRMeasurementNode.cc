/*******************************************************************************
 *
 * Copyright (c) 2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    Christoph Steup <steup@ivs.cs.uni-magdeburg.de>
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
 * $Id: $
 *
 ******************************************************************************/

#define F_CPU   16000000
#include "debug.h"
using namespace logging;

#include <avr/io.h>
#include <boost/mpl/vector.hpp>

#include <avr-halib/avr/InterruptManager/InterruptManager.h>
#include <avr-halib/avr/InterruptManager/SignalSemanticInterrupt.h>
#include <avr-halib/share/delay.h>
#include <avr-halib/avr/interrupt.h>
#include <avr-halib/regmaps/regmaps.h>
#include <avr-halib/avr/newTimer.h>
#include <avr-halib/regmaps/local.h>

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

template<>
inline UID getNodeID<void>(){
      static char name[]="QNode__";
      DDRG &= ~( (1<<PG0) | (1<<PG1) );
      PORTG = (1<<PG0) | (1<<PG1);
      name[6]=(PING & 0x3)+0x30;
      return UID(name);
}

struct config {
    typedef device::nic::CAN::avrCANARY can;
    typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
    typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
    typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;

    typedef famouso::mw::anl::AbstractNetworkLayer<
                nl,
                famouso::mw::afp::NoFragmentation,  // Fragmentation config: disable
                famouso::mw::afp::NoDefragmentation // Defragmentation config: disable
            > anl;
    typedef famouso::mw::el::EventLayer< anl > EL;
    typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
    typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
};


using avr_halib::drivers::Timer;
using avr_halib::config::TimerDefaultConfig;

typedef avr_halib::regmaps::local::Timer3 HighTimerRegMap;
typedef avr_halib::regmaps::local::Timer1 LowTimerRegMap;

struct LowTimerConfig : public TimerDefaultConfig<LowTimerRegMap>{

	typedef LowTimerRegMap Regmap;
	enum Parameters{
		overflowInt=false,
		ocmAInt=false,
		ocmBInt=false,
		ocmCInt=false,
		dynamicPrescaler=false
	};

	static const typename RegMap::WaveForms			waveForm = RegMap::FastPWMICR;
	static const typename RegMap::CompareMatchModes ocmCMode = RegMap::noOutput;
	static const typename RegMap::CompareMatchModes ocmBMode = RegMap::noOutput;
	static const typename RegMap::CompareMatchModes ocmAMode = RegMap::set;
	static const typename RegMap::Prescalers		ps		 = RegMap::ps1;
};

struct HighTimerConfig : public TimerDefaultConfig<HighTimerRegMap>{

	typedef HighTimerRegMap Regmap;
	enum Parameters{
		overflowInt=false,
		ocmAInt=false,
		ocmBInt=false,
		ocmCInt=false,
		dynamicPrescaler=false
	};

	static const typename RegMap::WaveForms			waveForm = RegMap::normal;
	static const typename RegMap::CompareMatchModes ocmCMode = RegMap::noOutput;
	static const typename RegMap::CompareMatchModes ocmBMode = RegMap::noOutput;
	static const typename RegMap::CompareMatchModes ocmAMode = RegMap::noOutput;
	static const typename RegMap::Prescalers		ps		 = RegMap::extClkFalling;
};


typedef Timer<LowTimerConfig> LowTimer;
typedef Timer<HighTimerConfig> HighTimer;

LowTimer  lowTimer;
HighTimer highTimer;

union Timestamp{
	uint32_t value;
	struct{
		uint16_t low;
		uint16_t high;
	};
};

struct Latency {
    volatile Timestamp timestampP;
    volatile Timestamp timestampN;
};

volatile Latency Lat;
volatile bool sync=false;
volatile bool sync2=false;

IMPLEMENT_INTERRUPT_SIGNALSEMANTIC_FUNCTION(publish){
    lowTimer.stop();
    Lat.timestampP.high=highTimer.getCounter();
	Lat.timestampP.low=lowTimer.getCounter();
    lowTimer.start();
    if (Lat.timestampP.low > 20) {
        PINF=1;
        sync2=true;
    }
}

IMPLEMENT_INTERRUPT_SIGNALSEMANTIC_FUNCTION(notify){
    lowTimer.stop();
    Lat.timestampN.high=highTimer.getCounter();
	Lat.timestampN.low=lowTimer.getCounter();
    lowTimer.start();
    if (Lat.timestampP.low > 20) {
	    sync=sync2;
        PINF=2;
    }
    sync2=false;
}

IMPLEMENT_INTERRUPT_SIGNALSEMANTIC_FUNCTION(canInt){
        device::nic::CAN::fireCANARYInterrupt();
}

struct InterruptConfig {
    typedef boost::mpl::vector<
                Interrupt::Slot<6,::Interrupt::Binding::SignalSemanticFunction>::Bind<&publish>,
				Interrupt::Slot<5,::Interrupt::Binding::SignalSemanticFunction>::Bind<&notify> ,
				Interrupt::Slot<18,::Interrupt::Binding::SignalSemanticFunction>::Bind<&canInt>
            >::type config;
};

typedef InterruptManager<InterruptConfig::config, false> IM;

using namespace logging;

int main(){

	IM::init();
    DDRF=3;

	lowTimer.setOutputCompareValue<LowTimer::unitA>(0x7FFF);
	ICR1=0xFFFF;
	lowTimer.start();
	highTimer.start();
	DDRB|=0x1<<5;
	DDRE&=~(1<<6);
	PORTE&=~(1<<6);

	UseRegmap(extInt, ExternalInterrupts);
	extInt.senseInt4=1;
	extInt.senseInt5=1;
	extInt.enableInt4=true;
	extInt.enableInt5=true;
	SyncRegmap(extInt);

	DDRE&=~( (0x1<<4) | (0x1<<5) );
	PORTE|=( (0x1<<4) | (0x1<<5) );

	sei();

    ::famouso::init< ::config>();

    ::config::PEC pec(famouso::mw::Subject("LatMete"));
    pec.announce();

    ::famouso::mw::Event e(pec.subject());
    e.length = 8;
    e.data = (uint8_t*)&Lat;
    CANGIE = 0;
    while(true)
	{
		while(!sync);
	    sync=false;
        pec.publish(e);
	}
	return 0;
}
