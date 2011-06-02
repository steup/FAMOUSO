/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner  <philipp.werner@st.ovgu.de>
 *               2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __COMMONINCLUDES_H_500E767DD18B93__
#define __COMMONINCLUDES_H_500E767DD18B93__

//-----------------------------------------------------------------------------
// NOTE: The real time channels have not been tested on AVR yet. Especially,
//       the provisional time framework may cause problems, as it is strongly
//       platform dependent. Reasoned by this FAMOUSO is configured to do not
//       check the publishing window. For test case and the measurement of
//       latencies is this ok but in order to use the real-time feature in
//       a productive mode a fully functional time framework for the AVR
//       platform is needed as well.
//-----------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------

#define F_CPU   16000000ULL
// Default logging
#include "debug.h"

// Global clock definition
#include "timefw/TimeSourceProvider.h"
#include "AVR_GlobalClock.h"
typedef GlobalAVRClock Clock;
FAMOUSO_TIME_SOURCE(Clock)

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

#include "mw/attributes/AttributeSet.h"
#include "mw/el/ml/ManagementLayer.h"

#include "mw/attributes/detail/AttributeSetProvider.h"
#include "mw/attributes/Period.h"
#include "mw/attributes/MaxEventLength.h"
#include "mw/attributes/RealTimeSlotStartBoundary.h"
#include "mw/attributes/RealTimeSlotEndBoundary.h"

#include "mw/anl/guard/NetworkGuard.h"
#include "mw/anl/guard/RT_NoWindowCheck.h"
#include "mw/anl/guard/NRT_HandledByNL.h"

namespace famouso {
    class config {
            typedef device::nic::CAN::avrCANARY can;
            typedef mw::nl::CAN::ccp::Client<can> ccpClient;
            typedef mw::nl::CAN::etagBP::Client<can> etagClient;
            typedef mw::nl::CANNL<can, ccpClient, etagClient> NL;
            typedef mw::guard::NetworkGuard<
                            NL,
                            mw::guard::RT_NoWindowCheck,
                            mw::guard::NRT_HandledByNL
                        > NG;
            typedef mw::anl::AbstractNetworkLayer<NG> ANL;

        public:
            typedef mw::el::EventLayer<ANL, mw::el::ml::ManagementLayer> EL;
            typedef mw::api::PublisherEventChannel<EL> PEC;
            typedef mw::api::SubscriberEventChannel<EL> SEC;
    };
}

template<>
inline UID getNodeID<void>(){
      static char name[]="QNode__";
      DDRG &= ~( (1<<PG0) | (1<<PG1) );
      PORTG = (1<<PG0) | (1<<PG1);
      name[6]=(PING & 0x3)+0x30;
      return UID(name);
}


#endif // __COMMONINCLUDES_H_500E767DD18B93__

