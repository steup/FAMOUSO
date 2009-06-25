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

#ifndef __PeakCAN_h__
#define __PeakCAN_h__


#if defined(LINUX)
    #include "devices/nic/can/peak/PeakDriverLinuxTrait.h"
#elif defined(WIN32)
    #include "devices/nic/can/peak/PeakDriverWindowsTrait.h"
#else
    #error "PEAK driver within FAMOUSO not support on that platform"
#endif

#include "devices/nic/can/PAXCAN/SimulateInterruptViaThreadAdapter.h"
#include "devices/nic/can/PAXCAN/PAXCAN.h"
#include "devices/nic/can/peak/PeakDriver.h"

namespace device {
    namespace nic {
        namespace CAN {

            /*! \brief PeakCAN is a typedef to a configured PeakCANDriver with enabled
             *         interrupt support. It is autmatically adapted to the platform
             *         it is compiled for. Supported platforms are Linux and Windows.
             */
            typedef SimulateInterruptViaThreadAdapter<PAXCAN<PeakDriver<PeakDriverTrait> > > PeakCAN;

        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

