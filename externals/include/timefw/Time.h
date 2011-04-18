/*******************************************************************************
 *
 * Copyright (c) 2010-2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __TIME_H_ECC196CA04CBCE__
#define __TIME_H_ECC196CA04CBCE__

#include <stdint.h>

namespace timefw {

    /*!
     * \brief Represents a time
     */
    template <typename time_type_>
    class TimeBase {

        protected:
            typedef time_type_ time_type;

            /// time in microseconds (us)
            time_type time;

            TimeBase(time_type t) :
                time(t) {
            }

        public:

            void add(const TimeBase & s) {
                time += s.time;
            }

            void sub(const TimeBase & s) {
                time -= s.time;
            }

            bool is_zero() {
                return time == 0;
            }

            bool operator ! () const {
                return !time;
            }

            bool operator < (const TimeBase & t2) const {
                return time < t2.time;
            }

            bool operator <= (const TimeBase & t2) const {
                return time <= t2.time;
            }

            bool operator > (const TimeBase & t2) const {
                return time > t2.time;
            }

            bool operator >= (const TimeBase & t2) const {
                return time >= t2.time;
            }

            bool operator == (const TimeBase & t2) const {
                return time == t2.time;
            }

            bool operator != (const TimeBase & t2) const {
                return time != t2.time;
            }

            void futurify(const TimeBase & period, const TimeBase & current) {
                time = increase_by_multiple_above(
                            time,
                            period.time,
                            current.time);

            }
    };

#ifndef __AVR__
    // Time in nanosec
    class Time : public TimeBase<uint64_t> {
            typedef TimeBase<uint64_t> Base;

        public:
            typedef Base::time_type time_type;

        protected:
            // t: Time in nanosec
            Time(time_type t) : Base(t) {
            }

        public:
            Time() : Base(0) {
            }

            static Time sec(time_type t) {
                return Time(t * 1000000000llu);
            }

            static Time msec(time_type t) {
                return Time(t * 1000000llu);
            }

            static Time usec(time_type t) {
                return Time(t * 1000llu);
            }

            static Time nsec(time_type t) {
                return Time(t);
            }

            void set_sec(time_type t) {
                time = t * 1000000000llu;
            }

            void set_msec(time_type t) {
                time = t * 1000000llu;
            }

            void set_usec(time_type t) {
                time = t * 1000llu;
            }

            void set_nsec(time_type t) {
                time = t;
            }

            time_type get_sec() const {
                return time / 1000000000llu;
            }

            time_type get_msec() const {
                return time / 1000000llu;
            }

            time_type get_usec() const {
                return time / 1000llu;
            }

            time_type get_nsec() const {
                return time;
            }

            Time & add(const Time & s) {
                Base::add(s);
                return *this;
            }

            Time & sub(const Time & s) {
                Base::sub(s);
                return *this;
            }
    };
#else
    // Time in usec
    class Time : public TimeBase<uint32_t> {
            typedef TimeBase<uint32_t> Base;

        public:
            typedef Base::time_type time_type;

        protected:
            // t: Time in usec
            Time(time_type t) : Base(t) {
            }

        public:
            Time() : Base(0) {
            }

            static Time sec(time_type t) {
                return Time(t * 1000000lu);
            }

            static Time msec(time_type t) {
                return Time(t * 1000lu);
            }

            static Time usec(time_type t) {
                return Time(t);
            }

            static Time nsec(time_type t) {
                return Time(t / 1000lu);
            }

            void set_sec(time_type t) {
                time = t * 1000000lu;
            }

            void set_msec(time_type t) {
                time = t * 1000lu;
            }

            void set_usec(time_type t) {
                time = t;
            }

            void set_nsec(time_type t) {
                time = t / 1000lu;
            }

            time_type get_sec() const {
                return time / 1000000lu;
            }

            time_type get_msec() const {
                return time / 1000lu;
            }

            time_type get_usec() const {
                return time;
            }

            /*
            time_type get_nsec() const {
                return time * 1000000llu;
            }
            */

            Time & add(const Time & s) {
                Base::add(s);
                return *this;
            }

            Time & sub(const Time & s) {
                Base::sub(s);
                return *this;
            }
    };
#endif

    Time operator-(const Time & t1, const Time & t2) {
        return Time(t1).sub(t2);
    }

    Time operator+(const Time & t1, const Time & t2) {
        return Time(t1).add(t2);
    }

} // namespace timefw


/*! \brief Operator to print a Time.
 *
 * \param out The output stream to print to.
 * \param t The Time to print.
 * \return The output stream.
 */
inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType & out, const timefw::Time & t) {
//inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType & out, timefw::Time t) {
    uint64_t us = t.get_usec();
    out << '(' << ::logging::log::dec << (us / 1000llu) << " ms, " << (us % 1000llu) << " us)";
    return out;
}

#endif // __TIME_H_ECC196CA04CBCE__


