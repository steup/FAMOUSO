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

#include <time.h>


// Real time logging
#define LOGGING_DEFINE_EXTENDED_OUTPUT_TYPE
#define LOGGING_DEFINE_OWN_OUTPUT_TYPE
#include "RealTimeLogger.h"
#include "logging/logging.h"
LOGGING_DEFINE_OUTPUT( ::logging::OutputLevelSwitchDisabled< ::logging::OutputStream< ::logging::RealTimeLogger<true> > > )


#include "mw/common/NodeID.h"
template <>
UID getNodeID<void>() {
    return UID("TimeMast");
}

#ifdef __XENOMAI__
#include "TimestampingXenomaiRTCAN.h"
#else
#include "devices/nic/can/SocketCAN/SocketCAN.h"
#endif
#include "mw/nl/can/etagBP/Broker.h"
#include "mw/nl/can/ccp/Broker.h"
#include "mw/nl/CANNL.h"

#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"
#include "famouso.h"


namespace famouso {
    class config {
#ifdef __XENOMAI__
            //typedef device::nic::CAN::XenomaiRTCAN can;
            typedef device::nic::CAN::SendTimestampingXenomaiRTCAN can;
#else
            typedef device::nic::CAN::SocketCAN can;
#endif
            typedef famouso::mw::nl::CAN::ccp::Broker<can> ccpBroker;
            typedef famouso::mw::nl::CAN::etagBP::Broker<can> etagBroker;
            typedef famouso::mw::nl::CANNL<can, ccpBroker, etagBroker> nl;
            typedef famouso::mw::anl::AbstractNetworkLayer<nl> ANL;
            typedef famouso::mw::el::EventLayer<ANL> BaseEL;
        public:
            typedef BaseEL EL;
            typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
            typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
    };
}


int main(int argc, char **argv) {
#ifdef __XENOMAI__
    const uint64_t interval_ns = 1000 * 1000 * 1000; // 1000 ms
    const clockid_t clock = CLOCK_REALTIME;

    uint64_t first_time = 0;            // Improve readablily of time
    uint64_t last_tx_time = 0;
    uint64_t last_tx_time_endian = 0;

    // Init famouso and time channel (NRT, contains last tx time in nano sec)
    famouso::init<famouso::config>(argc, argv);
    famouso::config::PEC time_channel("TimeSync");
    time_channel.announce();
    famouso::mw::Event event(time_channel.subject());
    event.length = 8;
    event.data = reinterpret_cast<uint8_t*>(&last_tx_time_endian); // in nano seconds

    // Init local time
    timespec wakeup_time;
    clock_gettime(clock, &wakeup_time);
    first_time = wakeup_time.tv_sec * 1000000000llu + wakeup_time.tv_nsec;

    while (1) {
        // Publish last tx time, get timestamp immediately after sending
        //::logging::log::emit() << "Event: " << ::logging::log::dec << last_tx_time << " ns\n";
        time_channel.publish(event);
#ifdef NO_SEND_TIMESTAMPING_DRIVER
        clock_gettime(clock, &wakeup_time);

        ::logging::log::emit() << "Event: " <<  ::logging::log::dec << last_tx_time <<
                " ns, next_last_tx " <<
                device::nic::CAN::SendTimestampingXenomaiRTCAN::last_timestamp() - first_time << " ns\n";
        // Update last tx time for next publishing
        last_tx_time = wakeup_time.tv_sec * 1000000000llu + wakeup_time.tv_nsec - first_time;
        last_tx_time_endian = htonll(last_tx_time);

#else
        // Update last tx time for next publishing
        last_tx_time = device::nic::CAN::SendTimestampingXenomaiRTCAN::last_timestamp() - first_time;
        last_tx_time_endian = htonll(last_tx_time);
#endif

        // Wait until time interval has passed
        wakeup_time.tv_nsec += interval_ns;
        wakeup_time.tv_sec += wakeup_time.tv_nsec / 1000000000llu;
        wakeup_time.tv_nsec %= 1000000000llu;

        clock_nanosleep(clock, TIMER_ABSTIME, &wakeup_time, 0);
    }
#else
    const uint64_t interval_ns = 500 * 1000 * 1000; // 500 ms
    const clockid_t clock = CLOCK_MONOTONIC;

    uint64_t global_time = 0;
    uint64_t global_time_endian = 0;

    // Init famouso and time channel
    famouso::init<famouso::config>();
    famouso::config::PEC time_channel("TimeSync");
    time_channel.announce();
    famouso::mw::Event event(time_channel.subject());
    event.length = 8;
    event.data = reinterpret_cast<uint8_t*>(&global_time_endian); // in nano seconds

    // Init local time
    timespec wakeup_time;
    clock_gettime(clock, &wakeup_time);

    while (1) {
        // Wait until time interval has passed
        wakeup_time.tv_nsec += interval_ns;
        wakeup_time.tv_sec += wakeup_time.tv_nsec / 1000000000;
        wakeup_time.tv_nsec %= 1000000000;

        while (clock_nanosleep(clock, TIMER_ABSTIME, &wakeup_time, 0) == EINTR)
            ;

        // Publish time
        time_channel.publish(event);

        // Update global time for next publishing
        global_time += interval_ns;
        global_time_endian = htonll(global_time);
    }
#endif
}


