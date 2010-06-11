/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                         Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
 *  \brief  Simple subscriber example using the core AFP API
 */

#define CPU_FREQUENCY 16000000

#include "util/Idler.h"

#include "mw/afp/Defragmentation.h"
#include "mw/afp/Config.h"
#include "mw/afp/shared/hexdump.h"

#include "famouso.h"

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


#ifndef AFP_CONFIG
// May be defined using a compiler parameter
#ifdef __AVR__
#define AFP_CONFIG 3
#else
#define AFP_CONFIG 1
#endif
#endif

using namespace famouso::mw;

#if (AFP_CONFIG == 1)
struct MyAFPDefragConfig : afp::DefaultConfig {
    enum {
        event_seq = true,
        reordering = true,
        duplicates = true,
        multiple_subjects = false,
        FEC = true
    };
};
#elif (AFP_CONFIG == 2)
struct MyAFPDefragConfig : afp::DefaultConfig {
    enum {
        event_seq = true,
        reordering = true,
        multiple_subjects = false
    };
};
#else
typedef afp::DefaultConfig MyAFPDefragConfig;
#endif


typedef afp::DefragmentationProcessor<MyAFPDefragConfig> DefragmentationProcessor;
typedef afp::DefragmentationStep<MyAFPDefragConfig> DefragmentationStep;


DefragmentationProcessor dp(16);



void CB(famouso::mw::api::SECCallBackData& cbd) {

#ifndef __AVR__
    ::logging::log::emit() << "Incoming fragment (" << cbd.length << " Bytes):\n";
    afp::shared::hexdump(cbd.data, cbd.length);
#endif

    DefragmentationStep ds(cbd.data, cbd.length);

    dp.process_fragment(ds);

    if (ds.event_complete()) {
        famouso::mw::Event e(cbd.subject);
        e.data = ds.get_event_data();
        e.length = ds.get_event_length();

        ::logging::log::emit() << "Incoming event (" << e.length << " Bytes):\n";
        afp::shared::hexdump(e.data, e.length);

        dp.event_processed(ds);
    }
}


int main(int argc, char **argv) {

    ::logging::log::emit() << "Started famouso init" << ::logging::log::endl;
    famouso::init<famouso::config>();
    ::logging::log::emit() << "Finished famouso init" << ::logging::log::endl;

#ifdef __AVR__
    sei();
#endif

    famouso::config::SEC sec("MTU___16");
    sec.callback.bind<CB>();
    sec.subscribe();

    Idler::idle();
}

