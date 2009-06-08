/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

// CAN-NL specific includes
#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/nl/can/etagBP/Broker.h"
#include "mw/nl/can/ccp/Broker.h"
#include "mw/nl/CANNL.h"

// UDPBroadCastNL specific
#include "mw/nl/UDPMultiCastNL.h"

// common famouso middlware includes
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/nal/NetworkAdapter.h"
#include "mw/gwl/Gateway.h"
#include "mw/el/EventLayer.h"
#include "mw/el/EventLayerMiddlewareStub.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "config/generator/description.h"

namespace famouso {

    namespace GW_CAN_UDPMC {
        class config {
            protected:
                typedef device::nic::CAN::PeakCAN can;
                typedef famouso::mw::nl::CAN::ccp::Broker<can> ccpBroker;
                typedef famouso::mw::nl::CAN::etagBP::Broker<can> etagBroker;
                typedef famouso::mw::nl::CANNL<can, ccpBroker, etagBroker> nlCAN;
                typedef famouso::mw::anl::AbstractNetworkLayer< nlCAN > anlCAN;

                typedef famouso::mw::nl::UDPMultiCastNL nlUDP;
                typedef famouso::mw::anl::AbstractNetworkLayer< nlUDP > anlUDP;

                typedef famouso::mw::nal::NetworkAdapter< anlCAN, anlUDP > na;
            public:
                typedef famouso::mw::el::EventLayer< na > EL;

                typedef famouso::mw::gwl::Gateway<EL> GW;
                typedef famouso::mw::el::EventLayerMiddlewareStub< EL > ELMS;
                typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;

                DESCRIPTION("Project: FAMOUSO\n"
                            "local Event Channel Handler with a configured gateway,\n"
                            "that connects a CAN network (PeakCAN-Driver) with an \n"
                            "UDP-Mutlicast network.\n\n"
                            "local Event Channel Handler\n"
                            "Author: Michael Schulze\n"
                            "Revision: $Rev$\n"
                            "$Date$\n"
                            "last changed by $Author$\n\n"
                            "build Time: "__TIME__"\n"
                            "build Date: "__DATE__"\n\n");

        };
    }

    typedef GW_CAN_UDPMC::config config;
}

#include "generic-main.impl"

