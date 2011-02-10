/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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


// Default logging
#include "logging/logging.h"

#include "TFW.h"
typedef famouso::time::GlobalClock<famouso::time::ClockDriverGPOS> Clock;
FAMOUSO_TIME_SOURCE(Clock)

#include "mw/common/NodeID.h"
template <>
UID getNodeID<void>() {
    return UID("RTSchedN");
}


#include "RTNetScheduler.h"

#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "devices/nic/can/SocketCAN/SocketCAN.h"
//#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/nl/CANNL.h"

//#include "mw/el/EventLayerClientStub.h"
#include "mw/nl/voidNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"

#include "mw/common/Event.h"
#include "famouso.h"



namespace famouso {
    class config {
            typedef device::nic::CAN::SocketCAN can;
            //typedef device::nic::CAN::PeakCAN can;
            typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
            typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
            typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> NL;
            //typedef famouso::mw::nl::voidNL NL;
            typedef famouso::mw::anl::AbstractNetworkLayer<NL> ANL;

            //typedef famouso::mw::el::EventLayerClientStub BaseEL;
        public:
            typedef famouso::mw::el::EventLayer<ANL> EL;
            typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
            typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
    };
}

#include "Dispatcher.h"

// TODO: rename to something with scheduler





int main(int argc, char ** argv) {
    famouso::init<famouso::config>();

    // can be integrated into famouso init
    famouso::config::SEC time_chan("TimeSync");
    typedef famouso::TimeSource::clock_type Clock;
    time_chan.callback.bind<Clock, &Clock::tick>(&famouso::TimeSource::instance());
    time_chan.subscribe();

    while (famouso::TimeSource::out_of_sync()) {
        usleep(1000);
    }
    ::logging::log::emit() << "Clock in sync\n";


    using namespace famouso::mw::rt_net_sched;
    RTNetScheduler<famouso::config::PEC, famouso::config::SEC> sched;
    NetworkSchedule can(famouso::mw::el::ml::NetworkID(/*"CAN0@1Mb"*/(uint64_t)0),
                        CanNetworkTimingConfig(
                            1000000,    // Bits per second
                            1000,       // Uhrengranularität in us
                            1000,       // Planungsgranularität in us
                            1000,       // Trigger bis USF in nano sec
                            1000        // Trigger bis USF in nano sec
                        ),
                        sched);

    // Idler would be sufficient at the moment
    ::logging::log::emit() << "Start dispatcher\n";
    Dispatcher::instance().run();
    ::logging::log::emit() << "Stop dispatcher\n";

    return 0;
}

