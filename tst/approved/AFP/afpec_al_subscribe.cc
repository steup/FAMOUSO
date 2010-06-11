/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

/*!
 *  \file
 *  \brief  Subscriber using AFP in Application Layer
 */

#define CPU_FREQUENCY 16000000

#include "mw/afp/AFPSubscriberEventChannel.h"
#include "mw/afp/Config.h"

#include "famouso.h"
#include "util/Idler.h"


#ifdef __AVR__

#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/nl/CANNL.h"
#include "devices/nic/can/at90can/avrCANARY.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"

namespace famouso {
    namespace CAN {
        class config {

                typedef device::nic::CAN::avrCANARY can;
                typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
                typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
                typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;
                typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
            public:
                typedef famouso::mw::el::EventLayer< anl > EL;
                typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
        };
    }

    typedef CAN::config config;
}
#else

#include "famouso_bindings.h"

#endif


using namespace famouso::mw;

struct AFPConfig : afp::DefaultConfig {
    enum {
        overflow_error_checking = false
    };
    typedef afp::MinimalSizeProp SizeProperties;
};



void ReceiveCallback(famouso::mw::api::SECCallBackData& cbd) {
    ::logging::log::emit() << cbd.data;
}

int main() {
    enum { mtu = 8 };
    ::logging::log::emit() << "Started famouso init" << ::logging::log::endl;
    famouso::init<famouso::config>();
    ::logging::log::emit() << "Finished famouso init" << ::logging::log::endl;

#ifdef __AVR__
    sei();
#endif

    afp::AFPSubscriberEventChannel<famouso::config::SEC, AFPConfig, Event> sec("MTU____8", mtu);
    sec.callback.bind<ReceiveCallback>();
    sec.subscribe();

    Idler::idle();

    return 0;
}

