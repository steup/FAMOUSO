/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                    Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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


#ifndef __TFW_H_76900890B02E04__
#define __TFW_H_76900890B02E04__


#include "debug.h"
#include "Singleton.h"

// Non-AVR
#include "boost/thread/xtime.hpp"
#include <time.h>
#include <errno.h>

#include "mw/common/Event.h"


namespace famouso {
namespace time {

    /*!
     * \brief Represents a time
     */
    class Time {

            // usec
            uint64_t time;

        public:
            Time(uint64_t time = 0) : time(time) {
            }

            Time & add_sec(unsigned int s) {
                time += s * 1000000;
                return *this;
            }

            Time & add_usec(unsigned int us) {
                time += us;
                return *this;
            }

            uint64_t get_usec() const {
                return time;
            }

            uint64_t get_sec() const {
                return time / 1000000;
            }

            Time operator + (const Time & t2) const {
                return Time(time + t2.time);
            }

            bool operator < (const Time & t2) const {
                return time < t2.time;
            }

            bool operator <= (const Time & t2) const {
                return time <= t2.time;
            }

            bool operator > (const Time & t2) const {
                return time > t2.time;
            }

            bool operator >= (const Time & t2) const {
                return time >= t2.time;
            }

            bool operator == (const Time & t2) const {
                return time == t2.time;
            }

            bool operator != (const Time & t2) const {
                return time != t2.time;
            }

            // Get representation for binary transmission (usec)
            uint64_t get() const {
                return time;
            }

            // Set from representation for binary transmission (usec)
            void set(uint64_t t) {
                time = t;
            }
    };

    // Non-AVR
    class ClockDriverGPOS {
        public:
            uint64_t current_local() {
                /*
                boost::xtime tmp;
                boost::xtime_get(&tmp, boost::TIME_UTC);
                return tmp.sec * 1000000 + tmp.nsec / 1000;
                */
                timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);
                return time.tv_sec * 1000000 + time.tv_nsec / 1000;
            }

            void wait_until(uint64_t local_time) {
                timespec wakeup_time;
                wakeup_time.tv_sec = local_time / 1000000;
                wakeup_time.tv_nsec = (local_time % 1000000) * 1000;
                while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wakeup_time, 0) == EINTR)
                    ;
            }
    };


    template <class ClockDriver>
    class LocalClock : public ClockDriver {
        public:
            uint64_t granularity() {
                return 1;
            }

            bool out_of_sync() {
                return false;
            }

            uint64_t current() {
                static uint64_t first = 0;
                uint64_t tmp2 = ClockDriver::current_local();
                if (!first) {
                    first = tmp2;
                    return 0;
                } else {
                    return tmp2 - first;
                }
            }

            void wait_until(uint64_t local_time) {
                ClockDriver::wait_until(local_time);
            }
    };

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

            void tick(const famouso::mw::Event & e) {
                // Simple sparse global time: message sets new global time
                last_tick_local = ClockDriver::current_local();

                FAMOUSO_ASSERT(e.length == 8);
                curr_global = ntohll(*reinterpret_cast<uint64_t*>(e.data));

                // TODO: get out of sync after rounds (assume pessimistic drift)
                sync = true;

                /*
                ::logging::log::emit<GlobalClock>() << "recv sync "
                    << Time(curr_global)
                    << " at local " << Time(last_tick_local) << "\n";
                    */
            }

            // TODO: wozu eigentlich? überdenken...
            uint64_t granularity() {
                return 0;
            }

            bool out_of_sync() {
                //printf("out_of_sync %d this %x\n", !sync, (void*)this);
                return !sync;
            }

            // Timestamp
            // TODO: Genauigkeit auf Granularität beschränken + zusätzliche high-res-Funktion für Zeitmessung?
            uint64_t current() {
                return local_to_global(ClockDriver::current_local());
            }

            void wait_until(uint64_t global_time) {
                // Assumption: time is not too far in future (the later the less
                //             accurate will be the wakeup in case of drifting clocks)
                ClockDriver::wait_until(global_to_local(global_time));
            }
    };

    // Programming Interface
    // TODO: maybe static + init in famouso-init is sufficient (save if)
    template <class Clock>
    class TimeSourceProvider : public Singleton<Clock> {
            typedef Singleton<Clock> Base;
        public:
            typedef Clock clock_type;

            static Time current() {
                return Time(Base::instance().current());
            }

            static Time current_local() {
                return Time(Base::instance().current_local());
            }

            static bool out_of_sync() {
                return Base::instance().out_of_sync();
            }

            static Time granularity() {
                return Time(Base::instance().granularity());
            }

            static void wait_until(const Time & time) {
                Base::instance().wait_until(time.get());
            }
    };


/**
 *  \brief  Defines the clock configuration
 *  \note   Only use it outside of any namespaces!!!
 */
#define FAMOUSO_TIME_SOURCE(CLOCK)                                      \
    namespace famouso {                                                 \
        typedef time::TimeSourceProvider< CLOCK > TimeSource;           \
    }

} // namespace time
} // namespace famouso


/*! \brief Operator to print a Time.
 *
 * \param out The output stream to print to.
 * \param t The Time to print.
 * \return The output stream.
 */
inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType & out, const famouso::time::Time & t) {
    uint64_t us = t.get_usec();
    //out << '(' << ::logging::log::dec << us << " us)";
    out << '(' << ::logging::log::dec << us / 1000 << " ms)";
    return out;
}


#endif // __TFW_H_76900890B02E04__

