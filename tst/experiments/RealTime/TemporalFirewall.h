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

#ifndef __TEMPORALFIREWALL_H_0154DA24DD73BA__
#define __TEMPORALFIREWALL_H_0154DA24DD73BA__

#include "mw/common/Event.h"
#include "timefw/Time.h"

namespace famouso {
namespace mw {
namespace api {
namespace detail {

        template <famouso::mw::Event::Type max_length>
        struct EventInfoStruct {
            /// Length of the event (is <= max_length)
            famouso::mw::Event::Type length;

            /// Event data buffer (may include attributes)
            uint8_t data[max_length];

            // Subject not needed

            /// Expire time of the event
            timefw::Time expire;

            /*!
             *  \brief  Constructor
             *  \note   Initial expire time in past, so initial temporal
             *          firewall content is invalid. This prevents delivery
             *          of an uninitialized event.
             */
            EventInfoStruct() : expire(0) {
            }
        };


#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

// AVR-Implementation with disabled interrupts
// Prevents race conditions during concurrent access by reader and writer.
// Assumes one writer and one reader
// Reader only gets to see a buffer updated by the writer if he calls read_unlock().
// "special concurrency/thread-safe container", no init of elements
template <class Data>
class TemporalFirewallDoubleBufferedBoost {

        typedef boost::lock_guard<boost::mutex> scoped_lock;

        boost::mutex mutex;
        Data buffer[2];
        Data * write;
        Data * read;
        bool reading;
        bool swap_after_reading;

        void swap() {
            Data * tmp = read;
            read = write;
            write = tmp;
        }

    public:
        TemporalFirewallDoubleBufferedBoost() :
            write(&buffer[0]),
            read(&buffer[1]),
            reading(false),
            swap_after_reading(false) {
        }

        Data & write_lock() {
            return *write;
        }

        void write_unlock() {
            scoped_lock lock(mutex);
            if (reading) {
                swap_after_reading = true;
            } else {
                swap();
            }
        }

        const Data & read_lock() {
            scoped_lock lock(mutex);
            reading = true;
            return *read;
        }

        void read_unlock() {
            scoped_lock lock(mutex);
            reading = false;
            if (swap_after_reading) {
                swap();
                swap_after_reading = false;
            }
        }
};

// AVR: Blocking geht nicht, weil aus Interrupt-Kontext beschrieben
// -> Double-Buffered
// für Publisher auch ohne Locking denkbar, wenn Nebenläufigkeit vermieden

} // namespace famouso
} // namespace mw
} // namespace api
} // namespace detail

#endif // __TEMPORALFIREWALL_H_0154DA24DD73BA__

