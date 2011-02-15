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

#ifndef __EVENT_TYPES_H_66AD9F3E324FEE__
#define __EVENT_TYPES_H_66AD9F3E324FEE__

#include <stdint.h>

namespace famouso {
    namespace mw {
        namespace el {
            namespace ml {

                /*! \todo   RealTimeSetResStateEvents could be extended to support
                 *          transmission of multiple state changes in one event.
                 *          This would save overhead (e.g. NodeID is only needed once).
                 */

                /*! \todo   For configurations with only one network a network ID is
                 *          not needed. Thus, network throughput can be saved by
                 *          adding further event types which do not contain network ID
                 *          data or omitting the transmission in another way.
                 */

                enum {
                    /// Announcements event (channel requirements)
                    announcements_event = 0,

                    /// Subscriptions event (channel requirements)
                    subscriptions_event = 1,

                    /// Real time reservation event (set RT producer state)
                    rt_reservation_event = 2,

                    /// Real time no reservation event (set RT producer state)
                    rt_no_reservation_event = 3,

                    /// Real time no subscriber event (set RT producer state)
                    rt_no_subscriber_event = 4,

                    /// Highest event type value (needed for get_event_type() implementation)
                    last_supported_event = 4,

                    /// Unsupported event
                    unsupported_event = 255
                };

                /*!
                 *  \brief  Returns event type
                 *  \param  data    Event data
                 *  \param  length  Event length
                 *  \return Event type or unsupported_event.
                 *
                 *  Reads event type from event data (first byte).
                 */
                static inline uint8_t get_event_type(const uint8_t * data, uint16_t length) {
                    if (!length || *data > last_supported_event)
                        return unsupported_event;
                    else
                        return *data;
                }

            } // namespace ml
        } // namespace el
    } // namespace mw
} // namespace famouso

#endif // __EVENT_TYPES_H_66AD9F3E324FEE__

