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

#include "mw/nl/CANNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "devices/nic/can/XenomaiRTCAN/XenomaiRTCAN.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/common/UID.h"
#include "famouso.h"
#include "debug.h"
#include "util/Idler.h"

namespace famouso {
    struct config {
        typedef device::nic::CAN::XenomaiRTCAN can;
        typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
        typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
        typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;
        typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
        typedef famouso::mw::el::EventLayer< anl > EL;
        typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
        typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
    };
}

typedef famouso::config::SEC SEC;

void cb(famouso::mw::api::SECCallBackData& cbd) {
    ::logging::log::emit()
        << "CallBack"
        << "Event Length=" << cbd.length
        << "Event Data="   << cbd.data
        << ::logging::log::endl;
}

int main(int argc, char **argv) {

    famouso::init<famouso::config>(argc, argv);
    SEC sec(famouso::mw::Subject(0xf100000000000000ull));
    sec.subscribe();
    sec.callback.bind<&cb>();
    SEC sec1(famouso::mw::Subject(0xf12));
    sec1.subscribe();
    sec1.callback.bind<&cb>();

    Idler::idle();
    return 0;
}
