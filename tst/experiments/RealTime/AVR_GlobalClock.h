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

#ifndef __AVRGLOBALCLOCK_H_6C5968787888BE__
#define __AVRGLOBALCLOCK_H_6C5968787888BE__

#include "timefw/Time.h"
#include "timefw/ClockDriverAVR.h"
#include "mw/common/Event.h"

    // interface: nanosec
    // internal: nanosec
    // Global = local time
    class GlobalAVRClock : public timefw::ClockDriverAVR {
            typedef timefw::ClockDriverAVR ClockDriver;
            bool sync;
        public:
            GlobalAVRClock() {
                last_macrotick = 0;
                sync = false;
            }

            void sync_event(const famouso::mw::Event & e) {
                FAMOUSO_ASSERT(e.length == 8);
                last_macrotick = ntohll(*reinterpret_cast<uint64_t*>(e.data));
                sync = true;
            }

            bool out_of_sync() {
                return sync;
            }

            // Timestamp in us
            uint64_t current() {
                return ClockDriver::current_local();
            }

            bool wait_until(uint64_t global_time) {
                // Assumption: time is not too far in future (the later the less
                //             accurate will be the wakeup in case of drifting clocks)
                return ClockDriver::wait_until(global_time);
            }
    };

#endif // __AVRGLOBALCLOCK_H_6C5968787888BE__

