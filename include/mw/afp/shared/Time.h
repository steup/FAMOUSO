/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                         Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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


#ifndef __TIME_H_2DD6595E115022__
#define __TIME_H_2DD6595E115022__


#ifdef __AVR__
// AVR version
//#warning Time class for AVR platform missing. Need timing framework replacing dummy implementation.

/*!
 * \brief Represents a time: dummy implementation
 */
class Time {
        int t;
    public:

        // Template function to hide warning if unused
        template <typename Time_Type>
        static void get_current_time(Time_Type & time) {
            int Warning__Time_class_for_AVR_platform_is_dummy_implementation__Need_timing_framework;
            static int curr = 0;
            time.t = curr;
            curr++;
        }

        template <typename Time_Type>
        static Time_Type current() {
            Time t;
            get_current_time(t);
            return t;
        }

        Time & add_sec(unsigned int s) {
            t += s;
            return *this;
        }

        uint64_t get_usec() const {
            return 0;
        }

        uint64_t get_sec() const {
            return t;
        }

        bool operator<(const Time & t2) const {
            return t < t2.t;
        }
};

#else
// General purpose OS version

#include "boost/thread/xtime.hpp"

#if 0

/*!
 * \brief Represents a time
 */
class Time {
        boost::xtime time;
    public:

        static void get_current_time(Time & time) {
            boost::xtime_get(&time.time, boost::TIME_UTC);
        }

        static Time current() {
            Time t;
            get_current_time(t);
            return t;
        }

        Time & add_sec(unsigned int s) {
            time.sec += s;
            return *this;
        }

        Time & add_usec(unsigned int us) {
            time.nsec += us * 1000;
            return *this;
        }

        uint64_t get_usec() const {
            return time.sec * 1000000 + time.nsec / 1000;
        }

        uint64_t get_sec() const {
            return time.sec;
        }

        bool operator<(const Time & t2) const {
            return boost::xtime_cmp(time, t2.time) < 0;
        }

#else

/*!
 * \brief Represents a time
 */
class Time {

        // usec since first current() call
        uint64_t time;

        uint64_t get_rep(boost::xtime & t) {
            return t.sec * 1000000 + t.nsec / 1000;
        }

    public:

        static void get_current_time(Time & t) {
            static uint64_t first = 0;
            boost::xtime tmp;
            boost::xtime_get(&tmp, boost::TIME_UTC);
            if (!first) {
                first = t.get_rep(tmp);
                t.time = 0;
            } else {
                t.time = t.get_rep(tmp) - first;
            }
        }

        static Time current() {
            Time t;
            get_current_time(t);
            return t;
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

        bool operator<(const Time & t2) const {
            return time < t2.time;
        }

        // Get representation for binary transmission
        uint64_t get() {
            return time;
        }

        // Set from representation for binary transmission
        void set(uint64_t t) {
            time = t;
        }
};

#endif

#endif

/*! \brief Operator to print a Time.
 *
 * \param out The output stream to print to.
 * \param t The Time to print.
 * \return The output stream.
 */
inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType & out, const Time & t) {
    uint64_t us = t.get_usec();
    out << '(' << ::logging::log::dec << us << " us)";
    return out;
}

#endif // __TIME_H_2DD6595E115022__

