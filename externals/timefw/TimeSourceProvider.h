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


#ifndef __TIMESOURCEPROVIDER_H_9B1DA15BF77AF7__
#define __TIMESOURCEPROVIDER_H_9B1DA15BF77AF7__

#include "timefw/Singleton.h"
#include "timefw/Time.h"


namespace timefw {

    /// Clock API
    // TODO: maybe static + static init() call is sufficient (save if)
    template <class Clock>
    class TimeSourceProvider : public Singleton<Clock> {
            typedef Singleton<Clock> Base;
        public:
            typedef Clock clock_type;

            /// Returns current (global) time
            static Time current() {
                return Base::instance().current();
            }

            /// Returns current local time
            static Time current_local() {
                return Base::instance().current_local();
            }

            /// Check whether out of sync
            static bool out_of_sync() {
                return Base::instance().out_of_sync();
            }

            /// Wait until given (global) time (returns false if interrupted by system signal)
            static bool wait_until(const Time & time) {
                return Base::instance().wait_until(time);
            }
    };

} // namespace timefw

/**
 *  \brief  Defines the clock configuration
 *  \note   Only use it outside of any namespaces!!!
 */
#define FAMOUSO_TIME_SOURCE(CLOCK)                                      \
    namespace timefw {                                                  \
        typedef TimeSourceProvider< CLOCK > TimeSource;                 \
    }


#endif // __TIMESOURCEPROVIDER_H_9B1DA15BF77AF7__


