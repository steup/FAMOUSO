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


#define LOGGING_OUTPUT_FILE "log_Publisher.txt"
#define LOGGING_DEFINE_EXTENDED_OUTPUT_TYPE
#define LOGGING_DEFINE_OWN_OUTPUT_TYPE
#include "RealTimeLogger.h"
#include "logging/logging.h"
LOGGING_DEFINE_OUTPUT( ::logging::OutputLevelSwitchDisabled< ::logging::OutputStream< ::logging::RTFileOutput > > )

#include "TFW.h"
typedef famouso::time::GlobalClock<famouso::time::ClockDriverGPOS> Clock;
FAMOUSO_TIME_SOURCE(Clock)

#include "mw/common/NodeID.h"
template <>
UID getNodeID<void>() {
    return UID("RT_Node1");
}


#include "SlotScheduler.h"
#include "NetworkParameters.h"

#include "mw/api/EventChannel.h"
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

#include "ManagementLayer.h"
#include "Attributes.h"

#include "mw/attributes/detail/AttributeSetProvider.h"
#include "mw/api/ExtendedEventChannel.h"


namespace famouso {
    class config {
            typedef device::nic::CAN::SocketCAN can;
            //typedef device::nic::CAN::PeakCAN can;
            typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
            typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
            typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> NL;
            //typedef famouso::mw::nl::voidNL NL;
            typedef famouso::mw::anl::AbstractNetworkLayer<NL> ANL;
            typedef famouso::mw::el::EventLayer<ANL> BaseEL;

            //typedef famouso::mw::el::EventLayerClientStub BaseEL;
        public:
            typedef ManagementLayer<BaseEL> EL;
            typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
            typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
    };
}

#include "Dispatcher.h"
#include "RealTimePublisherEventChannel.h"

// Output of UIDs
#include "mw/nl/awds/logging.h"



typedef famouso::mw::attributes::detail::SetProvider<
         famouso::mw::attributes::Period<200000>,
         MaxEventSize<10000>,
         RealTime
        >::attrSet rt_req;

class PEC3 : public RealTimePublisherEventChannel<famouso::config::PEC, rt_req> {
        Task pub_task;
        Event event;
    public:
        typedef RealTimePublisherEventChannel<famouso::config::PEC, rt_req> Base;

        PEC3() :
            RealTimePublisherEventChannel<famouso::config::PEC, rt_req>("rt__subj"),
            event(subject())
        {
            pub_task.start = TimeSource::current().add_usec(500000);
            pub_task.period = get_period();
            pub_task.function.bind<PEC3, &PEC3::publish_task>(this);
            Dispatcher::instance().enqueue(pub_task);
        }

        void publish_task() {
            event.data = (uint8_t *)"1234567890";
            event.length = 10;
            publish(event);
        }
};


void subscriber_callback(const famouso::mw::api::SECCallBackData& event) {
    ::logging::log::emit() << "callback: subject " << event.subject
                           << ", length " << ::logging::log::dec << event.length
                           << ", data " << event.data
                           << ", time " << famouso::TimeSource::current()
                           << logging::log::endl;
}

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
    printf("Clock in sync\n");


    famouso::config::SEC sec1("rt__subj");
    sec1.callback.bind<&subscriber_callback>();
    sec1.subscribe();

    famouso::config::PEC pec1("01234567");
    pec1.announce();

//    {
    PEC3 pec2;
    pec2.announce();
//    }

    printf("Start dispatcher\n");
    Dispatcher::instance().run();

    return 0;
}

