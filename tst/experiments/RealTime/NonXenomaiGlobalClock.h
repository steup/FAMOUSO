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


#ifndef __NONXENOMAIGLOBALCLOCK_H_71A80D5413BBD4__
#define __NONXENOMAIGLOBALCLOCK_H_71A80D5413BBD4__

#include "debug.h"
#include "mw/common/Event.h"
#include "timefw/Time.h"


// Simple global time: message sets new global time, assumes negilable message latency
// offset-correction, garantiert nicht die monotonie und stetigkeit der zeit...
template <class ClockDriver>
class GlobalClock : public ClockDriver {
        uint64_t curr_global;

        volatile bool sync;

        uint64_t last_tick_local;


        uint64_t local_to_global(uint64_t local) {
            return curr_global + (local - last_tick_local);
        }

        uint64_t global_to_local(uint64_t global) {
            return global - curr_global + last_tick_local;
        }

    public:
        /*! \brief delivers the current %level of %logging */
        static ::logging::Level::levels level() {
            return ::logging::Level::user;
        }
        /*! \brief delivers the string reporting the current %level of %logging */
        static const char * desc() {
            return "[ CLOCK  ] ";
        }

        GlobalClock() : curr_global(0), sync(false), last_tick_local(0) {
        }

        void sync_event(const famouso::mw::Event & e) {
            last_tick_local = ClockDriver::current_local();

            FAMOUSO_ASSERT(e.length == 8);
            curr_global = ntohll(*reinterpret_cast<uint64_t*>(e.data));

            // TODO: get out of sync after rounds (assume pessimistic drift)
            sync = true;

#ifdef CLOCK_ACCURACY_OUTPUT
            ::logging::log::emit<GlobalClock>() << "recv sync "
                << timefw::Time(curr_global)
                << " at local " << timefw::Time(last_tick_local) << "\n";
#endif
        }

        bool out_of_sync() {
            //printf("out_of_sync %d this %x\n", !sync, (void*)this);
            return !sync;
        }

        // Timestamp
        uint64_t current() {
            return local_to_global(ClockDriver::current_local());
        }

        bool wait_until(uint64_t global_time) {
            // Assumption: time is not too far in future (the later the less
            //             accurate will be the wakeup in case of drifting clocks)
            return ClockDriver::wait_until(global_to_local(global_time));
        }
};


#endif // __NONXENOMAIGLOBALCLOCK_H_71A80D5413BBD4__


