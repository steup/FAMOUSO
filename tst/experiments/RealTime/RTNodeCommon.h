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

#ifndef __RTNODEGENERIC_H_C03CF16E5BBBB4__
#define __RTNODEGENERIC_H_C03CF16E5BBBB4__

#if 1
// Real time logging
#define LOGGING_OUTPUT_FILE "log_" FAMOUSO_NODE_ID ".txt"
#define LOGGING_DEFINE_EXTENDED_OUTPUT_TYPE
#define LOGGING_DEFINE_OWN_OUTPUT_TYPE
#include "RealTimeLogger.h"
#include "logging/logging.h"
LOGGING_DEFINE_OUTPUT( ::logging::OutputLevelSwitchDisabled< ::logging::OutputStream< ::logging::RTFileOutput > > )
#else
// Default logging
#include "logging/logging.h"
#endif

#include "TFW.h"
typedef famouso::time::GlobalClock<famouso::time::ClockDriverGPOS> Clock;
FAMOUSO_TIME_SOURCE(Clock)

#include "mw/common/NodeID.h"
template <>
UID getNodeID<void>() {
    return UID(FAMOUSO_NODE_ID);
}


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

#include "mw/el/ml/ManagementLayer.h"
#include "mw/el/EventLayer.h"

#include "mw/attributes/detail/AttributeSetProvider.h"
#include "mw/api/ExtendedEventChannel.h"
#include "guard/NetworkGuard.h"
#include "guard/RT_WindowCheck.h"
#include "guard/RT_NoWindowCheck.h"
#include "guard/NRT_HandledByNL.h"
#include "guard/NRT_PollSlave.h"

namespace famouso {
    class config {
            typedef device::nic::CAN::SocketCAN can;
            //typedef device::nic::CAN::PeakCAN can;
            typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
            typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
            typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> NL;
            //typedef famouso::mw::nl::voidNL NL;
            typedef famouso::mw::guard::NetworkGuard<
                            NL,
                            famouso::mw::guard::RT_WindowCheck,
                            famouso::mw::guard::NRT_HandledByNL
                        > NG;
            typedef famouso::mw::anl::AbstractNetworkLayer<NG> ANL;

            //typedef famouso::mw::el::EventLayerClientStub BaseEL;
        public:
            typedef famouso::mw::el::EventLayer<ANL, famouso::mw::el::ml::ManagementLayer> EL;
            typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
            typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
    };
}

#include "Dispatcher.h"
#include "RealTimePublisherEventChannel.h"



#endif // __RTNODEGENERIC_H_C03CF16E5BBBB4__

