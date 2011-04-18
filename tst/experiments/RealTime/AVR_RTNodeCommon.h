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

#ifndef __RTAVRNODECOMMON_H_0E9783D55B8B9B__
#define __RTAVRNODECOMMON_H_0E9783D55B8B9B__

//-----------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------


// RT-PEC output verbosity: 0 - no output, 1 - only status changes, 2 - all activities
#define RTPEC_OUTPUT 0
#define RTSEC_OUTPUT 0


//-----------------------------------------------------------------------------


// Default logging
#include "debug.h"
//#include "logging/logging.h"

#include "avr-halib/share/delay.h"
static inline bool usleep(uint32_t t) {
    delay_ms(t / 1000lu);
    return true;
}

#ifndef TEST_AVR_NRT
// Global clock definition
#include "timefw/TimeSourceProvider.h"
#include "AVR_GlobalClock.h"
typedef GlobalAVRClock Clock;
FAMOUSO_TIME_SOURCE(Clock)
#endif


// Node ID definition
#include "mw/common/NodeID.h"
template <>
UID getNodeID<void>() {
    return UID(FAMOUSO_NODE_ID);
}


#include "mw/api/EventChannel.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "devices/nic/can/at90can/avrCANARY.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/nl/CANNL.h"

#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"

#include "mw/common/Event.h"
#include "famouso.h"

#include "mw/el/EventLayer.h"

#ifndef TEST_AVR_NRT
#include "mw/el/ml/ManagementLayer.h"
#include "mw/attributes/detail/AttributeSetProvider.h"
#include "mw/attributes/Period.h"
#include "mw/attributes/MaxEventLength.h"
#include "mw/attributes/RealTimeSlotStartBoundary.h"
#include "mw/attributes/RealTimeSlotEndBoundary.h"
#include "guard/NetworkGuard.h"
#include "guard/RT_WindowCheck.h"
#include "guard/RT_NoWindowCheck.h"
#include "guard/NRT_HandledByNL.h"
#endif

namespace famouso {
    class config {
            typedef device::nic::CAN::avrCANARY can;
            typedef mw::nl::CAN::ccp::Client<can> ccpClient;
            typedef mw::nl::CAN::etagBP::Client<can> etagClient;
            typedef mw::nl::CANNL<can, ccpClient, etagClient> NL;
#ifndef TEST_AVR_NRT
            typedef mw::guard::NetworkGuard<
                            NL,
                            mw::guard::RT_WindowCheck,
                            mw::guard::NRT_HandledByNL
                        > NG;
            typedef mw::anl::AbstractNetworkLayer<NG> ANL;
#else
            typedef mw::anl::AbstractNetworkLayer<NL> ANL;
#endif

        public:
#ifndef TEST_AVR_NRT
            typedef mw::el::EventLayer<ANL, mw::el::ml::ManagementLayer> EL;
#else
            typedef mw::el::EventLayer<ANL> EL;
#endif
            typedef mw::api::PublisherEventChannel<EL> PEC;
            typedef mw::api::SubscriberEventChannel<EL> SEC;
    };
}


#ifndef TEST_AVR_NRT

#include "timefw/Dispatcher.h"

#define CLOCK_SYNC_INIT \
    famouso::config::SEC time_chan("TimeSync"); \
    typedef timefw::TimeSource::clock_type Clock; \
    time_chan.callback.bind<Clock, &Clock::sync_event>(&timefw::TimeSource::instance()); \
    time_chan.subscribe(); \
    while (timefw::TimeSource::out_of_sync()) { \
    }   \
    ::logging::log::emit() << "Clock in sync\n";

#endif


#endif // __RTAVRNODECOMMON_H_0E9783D55B8B9B__

