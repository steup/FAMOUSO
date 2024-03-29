/*******************************************************************************
 *
 * Copyright (c) 2010-2011 Philipp Werner <philipp.werner@st.ovgu.de>
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


#ifndef __LOCALCLOCK_H_68A2D13D1E1687__
#define __LOCALCLOCK_H_68A2D13D1E1687__

#include "Time.h"
#include <stdint.h>

namespace timefw {

    /// Local clock
    template <class ClockDriver>
    class LocalClock : public ClockDriver {

            // Only used to increse readablity, may be a configuration parameter
            Time first;

        public:
            LocalClock() : first() {
            }

            /// Returns whether the clock is out of sync
            bool out_of_sync() {
                return false;
            }

            /// Returns current time in nanosecs
            Time current() {
                Time tmp2 = ClockDriver::current_local();
                if (!first) {
                    first = tmp2;
                    return Time();
                } else {
                    return tmp2 - first;
                }
            }

            /// Waits until local_time (given in nanosecs)
            bool wait_until(const Time & local_time) {
                if (!first)
                    current();
                return ClockDriver::wait_until(local_time + first);
            }
    };

} // namespace timefw

#endif // __LOCALCLOCK_H_68A2D13D1E1687__


