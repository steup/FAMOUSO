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

#ifndef __AVR__
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#endif

namespace famouso {
    namespace mw {
        namespace api {
            namespace detail {

                /// Event information that is stored in a temporal firewall
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
                    EventInfoStruct() : expire() {
                    }
                };

                /// Temporal firewall without mutal exclusion
                template <class Data>
                class TemporalFirewallThreadUnsafe {
                        Data buffer;

                    public:
                        Data & write_lock() {
                            return buffer;
                        }

                        void write_unlock() {
                        }

                        const Data & read_lock() {
                            return buffer;
                        }

                        void read_unlock() {
                        }
                };

#ifdef __AVR__
                /*!
                 *  \brief  Temporal firewall with mutual exclusion
                 *          (double buffered with atomic switch)
                 *  \tparam Data    Data type to store
                 *
                 *  Prevents race conditions during concurrent access by reader and writer,
                 *  but allows concurrent reading and writing.
                 *  Reader only gets to see a buffer that was updated by the writer if he
                 *  calls read_unlock(). So read and write locks should be only hold as
                 *  long as necessary.
                 *
                 *  \note   Assumes there are only one writer and one reader.
                 */
                template <class Data>
                class TemporalFirewallDoubleBuffered {

                        // Locking by disabling interrupts
                        struct scoped_lock {
                            char sreg;
                            scoped_lock() {
                                sreg = SREG;
                                cli();
                            }
                            ~scoped_lock() {
                                SREG = sreg;
                            }
                        };

                        Data buffer[2];

                        enum {
                            buffer_mask = 0x1,
                            reading = 0x2,
                            swap_after_reading = 0x4
                        };
                        uint8_t state;

                    public:
                        /// Constructor
                        TemporalFirewallDoubleBuffered() :
                            state(0) {
                        }

                        /// Lock for writing
                        Data & write_lock() {
                            return buffer[state & buffer_mask];
                        }

                        /// Undo write_lock()
                        void write_unlock() {
                            scoped_lock lock;
                            if (state & reading) {
                                state |= swap_after_reading;
                            } else {
                                state ^= buffer_mask;   // Swap buffers
                            }
                        }

                        /// Lock for reading
                        const Data & read_lock() {
                            scoped_lock lock;
                            state |= reading;
                            return buffer[(state ^ buffer_mask) & buffer_mask];
                        }

                        /// Undo read_lock()
                        void read_unlock() {
                            scoped_lock lock;
                            state &= ~reading;
                            if (swap_after_reading) {
                                state ^= buffer_mask;   // Swap buffers
                                state &= ~swap_after_reading;
                            }
                        }
                };

#else

                /*!
                 *  \brief  Temporal firewall with mutual exclusion
                 *          (double buffered with atomic switch)
                 *  \tparam Data    Data type to store
                 *
                 *  Prevents race conditions during concurrent access by reader and writer,
                 *  but allows concurrent reading and writing.
                 *  Reader only gets to see a buffer that was updated by the writer if he
                 *  calls read_unlock(). So read and write locks should be only hold as
                 *  long as necessary.
                 *
                 *  \note   Assumes there are only one writer and one reader.
                 */
                template <class Data>
                class TemporalFirewallDoubleBuffered {

                        typedef boost::lock_guard<boost::mutex> scoped_lock;

                        boost::mutex mutex;
                        Data buffer[2];
                        Data * write;
                        Data * read;
                        bool reading;
                        bool swap_after_reading;

                        /// Swap buffers
                        void swap() {
                            Data * tmp = read;
                            read = write;
                            write = tmp;
                        }

                    public:
                        /// Constructor
                        TemporalFirewallDoubleBuffered() :
                            write(&buffer[0]),
                            read(&buffer[1]),
                            reading(false),
                            swap_after_reading(false) {
                        }

                        /// Lock for writing
                        Data & write_lock() {
                            return *write;
                        }

                        /// Undo write_lock()
                        void write_unlock() {
                            scoped_lock lock(mutex);
                            if (reading) {
                                swap_after_reading = true;
                            } else {
                                swap();
                            }
                        }

                        /// Lock for reading
                        const Data & read_lock() {
                            scoped_lock lock(mutex);
                            reading = true;
                            return *read;
                        }

                        /// Undo read_lock()
                        void read_unlock() {
                            scoped_lock lock(mutex);
                            reading = false;
                            if (swap_after_reading) {
                                swap();
                                swap_after_reading = false;
                            }
                        }
                };
#endif


            } // namespace detail
        } // namespace api
    } // namespace mw
} // namespace famouso

#endif // __TEMPORALFIREWALL_H_0154DA24DD73BA__

