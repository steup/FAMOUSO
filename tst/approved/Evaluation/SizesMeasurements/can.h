/*******************************************************************************
 *
 * Copyright (c) 2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
 * $Id: $
 *
 ******************************************************************************/

#ifndef __CAN_H_135FD2D6F9EA9A__
#define __CAN_H_135FD2D6F9EA9A__

#define F_CPU 16000000ULL

#include "famouso.h"

#include "mw/nl/CANNL.h"
#include "devices/nic/can/at90can/avrCANARY.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"

#include "mw/afp/Config.h"
#include "mw/anl/AbstractNetworkLayer.h"


#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "mw/el/ml/ManagementLayer.h"
#include "mw/el/EventLayer.h"


#ifdef RT_FEATURE
// Global clock definition
#include "timefw/TimeSourceProvider.h"
#include "AVR_GlobalClock.h"
typedef GlobalAVRClock Clock;
FAMOUSO_TIME_SOURCE(Clock)

#include "mw/anl/guard/NetworkGuard.h"
#include "mw/anl/guard/RT_WindowCheck.h"
#include "mw/anl/guard/RT_NoWindowCheck.h"
#include "mw/anl/guard/NRT_HandledByNL.h"
#include "mw/anl/guard/NRT_PollSlave.h"
#endif

struct MinAFPConfig : famouso::mw::afp::DefaultConfig {
    typedef object::OneBlockAllocator<famouso::mw::afp::EmptyType, 255> Allocator;
    typedef famouso::mw::afp::MinimalSizeProp SizeProperties;
    enum {
        overflow_error_checking = false
    };
};

#endif // __CAN_H_135FD2D6F9EA9A__

