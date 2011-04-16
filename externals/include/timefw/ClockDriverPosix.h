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


#ifndef __CLOCKDRIVERPOSIX_H_079AA6945D312F__
#define __CLOCKDRIVERPOSIX_H_079AA6945D312F__

#include <time.h>
#include <errno.h>

namespace timefw {

    /// Clock driver for posix environment
    class ClockDriverPosix {
        public:
            /// Returns current local time in nanosecs
            uint64_t current_local() {
                timespec time;
                clock_gettime(CLOCK_REALTIME, &time);
                return time.tv_sec * 1000000000LLU + time.tv_nsec;
            }

            /*! \brief Sleeps until local_time (given in nanosecs)
             *  \return Whether waiting was not interrupted by a system signal
             */
            bool wait_until(uint64_t local_time) {
                uint64_t curr = current_local();
                if (curr < local_time) {
                    // Only wait if time is in future
                    timespec wakeup_time;
                    wakeup_time.tv_sec = local_time / 1000000000LLU;
                    wakeup_time.tv_nsec = local_time % 1000000000LLU;
                    if (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &wakeup_time, 0) != 0)
                        return false;
                }
                return true;
            }
    };

} // namespace timefw

#endif // __CLOCKDRIVERPOSIX_H_079AA6945D312F__


