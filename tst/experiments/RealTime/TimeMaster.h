/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner <philipp.werner@st.ovgu.de>
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
#include "mw/common/Event.h"
#include "timefw/Dispatcher.h"

#if 0
// Publishes timestamp in nanosec
template <class PEC>
class TimeMaster {
        PEC time_channel;

        const uint64_t interval_us;
        const clockid_t clock;

        uint64_t first_time;            // Improve readablily of time
        uint64_t last_tx_time;
        uint64_t last_tx_time_endian;

        famouso::mw::Event event;

    public:
        timefw::Task time_master_task;

        TimeMaster() :
            time_channel("TimeSync"),
            interval_us(1000 * 1000), // 1000 ms
            clock(CLOCK_REALTIME),
            first_time(timefw::TimeSource::current().get_usec() + 100),
            last_tx_time(0),
            last_tx_time_endian(0),
            event(time_channel.subject()),
            time_master_task(timefw::Time::usec(first_time), timefw::Time::usec(interval_us), false)
        {
            time_channel.announce();
            event.length = 8;
            event.data = reinterpret_cast<uint8_t*>(&last_tx_time_endian);
            time_master_task.template bind<TimeMaster, &TimeMaster::publish_sync>(this);
            timefw::Dispatcher::instance().enqueue(time_master_task);
        }

        void publish_sync() {
            time_channel.publish(event);
#ifndef SEND_TIMESTAMPING_DRIVER
            // Assumption: no time between tx interrupt and following timestamp
            // Update last tx time for next publishing
            last_tx_time = timefw::TimeSource::instance().current().get_nsec();
            last_tx_time_endian = htonll(last_tx_time);

#else
            // Update last tx time for next publishing
            last_tx_time = device::nic::CAN::SendTimestampingXenomaiRTCAN::last_timestamp() - first_time;
            last_tx_time_endian = htonll(last_tx_time);
#endif
        }

};

#else
#include "RealTimePublisherEventChannel.h"

typedef famouso::mw::attributes::detail::SetProvider<
             famouso::mw::attributes::Period<1000*1000>,
             famouso::mw::attributes::MaxEventLength<8>/*,
             famouso::mw::attributes::RealTimeSlotStartBoundary<2000>,
             famouso::mw::attributes::RealTimeSlotEndBoundary<10000>*/
        >::attrSet RTM_Req;

template <class PEC>
class TimeMaster : protected famouso::mw::api::RealTimePublisherEventChannel<PEC, RTM_Req> {
        typedef famouso::mw::api::RealTimePublisherEventChannel<PEC, RTM_Req>  Base;

        const clockid_t clock;

        uint64_t last_tx_time;
        uint64_t last_tx_time_endian;

        famouso::mw::Event event;

    public:

        TimeMaster() :
            Base("TimeSync"),
            clock(CLOCK_REALTIME),
            last_tx_time(0),
            last_tx_time_endian(0),
            event(Base::subject())
        {
            Base::announce();
            event.length = 8;
            event.data = reinterpret_cast<uint8_t*>(&last_tx_time_endian);
            // Use deliver task, because we need send timestamp
            Base::deliver_task.template bind<TimeMaster, &TimeMaster::publish_sync>(this);
        }

        void publish_sync() {
            Base::deliver_to_net(event);
#ifndef SEND_TIMESTAMPING_DRIVER
            // Assumption: no time between tx interrupt and following timestamp
            // Update last tx time for next publishing
            last_tx_time = timefw::TimeSource::instance().current().get_nsec();
            last_tx_time_endian = htonll(last_tx_time);

#else
#error SEND_TIMESTAMPING_DRIVER currently unsupported
#endif
        }

};
#endif

