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
    class Time {

            /// time in microseconds (us)
            uint64_t time;

        public:
            Time(uint64_t time_us = 0) : time(time_us) {
            }

            Time & add_sec(uint64_t s) {
                time += s * 1000000;
                return *this;
            }

            Time & add_usec(uint64_t us) {
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


