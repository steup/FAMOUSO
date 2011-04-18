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


#ifndef __CLOCKDRIVERAVR_H_FD2661D56BE87D__
#define __CLOCKDRIVERAVR_H_FD2661D56BE87D__

#if defined(__AVR_AT90CAN128__)
#   include "avr-halib/portmaps/robbyboard.h"
#else
#   error "Not ported to this platform yet."
#endif

#include "avr-halib/avr/timer.h"

// Interrupt needed for EggTimer<Timer2>
UseInterrupt(SIG_OUTPUT_COMPARE2);



namespace timefw {

    /// Clock driver for posix environment
    class ClockDriverAVR {
        protected:
            uint64_t last_macrotick;

            void tick() {
                last_macrotick += granularity_ns;
                timer.start(1);
            }

            ExactEggTimer<Timer2> timer;

        public:
            enum {
                granularity_ns = 1000llu * 1000llu // 1000 us = 1 ms
            };

            ClockDriverAVR() {
                timer.onTimerDelegate.bind<ClockDriverAVR, & ClockDriverAVR::tick> (this);
                timer.start(1);
            }

            /// Returns current local time in nanosecs
            uint64_t current_local() {
                return last_macrotick;
            }

            /*! \brief Sleeps until local_time (given in nanosecs)
             *  \return Whether waiting was not interrupted by a system signal
             */
            bool wait_until(uint64_t local_time) {
                while (current_local() < local_time)
                    ;
                return true;
            }
    };

} // namespace timefw


#endif // __CLOCKDRIVERAVR_H_FD2661D56BE87D__

