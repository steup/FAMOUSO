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

#ifndef __REALTIMESUBSCRIBEREVENTCHANNEL_H_1FC86AAEB977E9__
#define __REALTIMESUBSCRIBEREVENTCHANNEL_H_1FC86AAEB977E9__

#include "debug.h"
#include "Dispatcher.h"

#include "mw/common/Event.h"
#include "mw/api/CallBack.h"
#include "mw/el/ml/ChannelReservationData.h"
#include "mw/el/ml/NetworkID.h"

#include "mw/attributes/Period.h"
#include "mw/attributes/MaxEventLength.h"

#include "TemporalFirewall.h"

#include <string.h>

namespace famouso {
    namespace mw {
        namespace api {

            template <typename EC, typename Requirement,
                      template <class> class TemporalFirewall = detail::TemporalFirewallDoubleBufferedBoost>
            class RealTimeSubscriberEventChannel : public EC {
                    typedef RealTimeSubscriberEventChannel type;

                protected:
                    typedef typename Requirement::template find_ct< attributes::Period<0> >::type PeriodAttrib;
                    typedef typename Requirement::template find_ct< attributes::MaxEventLength<0> >::type MELAttrib;

                    enum {
                        /// Period given in requirement attributes
                        period = PeriodAttrib::value,

                        /// Maximum event length given in requirement attributes
                        mel = MELAttrib::value
                    };

                    /// Event information struct
                    typedef detail::EventInfoStruct<mel> EventInfo;

                    /// Temporal firewall containing event information
                    TemporalFirewall<EventInfo> tf;


                    // logging helper
                    struct RT {
                        /*! \brief delivers the current %level of %logging */
                        static ::logging::Level::levels level() {
                            return ::logging::Level::user;
                        }
                        /*! \brief delivers the string reporting the current %level of %logging */
                        static const char * desc() {
                            return "[ RT SUB ] ";
                        }
                    };

                public:
                    RealTimeSubscriberEventChannel(const Subject& subject) : EC(subject) {
                        EC::trampoline.template bind<type, &type::trampoline_impl>(this);
                        EC::callback.template bind<type, &type::callback_periodic_notify_impl>(this);
                    }

                private:

                    typedef typename EC::Action Action;

                    uint16_t trampoline_impl(Action & action) {
                        if (action.action == Action::get_requirements) {
                            // Return requirement attribute set
                            if (action.buffer) {
                                new (action.buffer) Requirement;
                            }
                            return Requirement::overallSize;
                        }
                    }

                    void callback_periodic_notify_impl(const Event & event) {
                        // TODO: Deadline prüfen, Filter anwenden, Strength prüfen (z.B. previous_strength als member-var zwischenspeichern)...
                        FAMOUSO_ASSERT(event.length <= 1000 /* TODO: MEL */);
                        EventInfo * ei = tf.write_lock();
                        // TODO: length und subject kopieren!!!
                        memcpy(ei->data, event.data, event.length);
                        ei->length = event.length;
                        ei->expire = TimeSource::current().add_usec(period); // oder deadline
                        tf.write_unlock();
                        return 0;
                    }

                    // may run in another thread
                    void notify_task() const {
                        // TODO:Omission beachten
                        EventInfo & ei = tf.read_lock();
                        if (TimeSource::current() < ei.expire) {
                            // expire in future
                            ::logging::log::emit<RT>()
                                << "update notify: chan "
                                << el::ml::LocalChanID(reinterpret_cast<uint64_t>(this))
                                << " at " << TimeSource::current() << " expiring at " << ei.expire << "\n";
                            //post(true, callback);
                        } else {
                            // expire in past
                            ::logging::log::emit<RT>()
                                << "exception notify: chan "
                                << el::ml::LocalChanID(reinterpret_cast<uint64_t>(this))
                                << " at " << TimeSource::current() << " EXPIRED at " << ei.expire << "\n";
                            //post(true, exception);
                        }
                        tf.read_unlock();
                    }

            };

        } // namespace api
    } // namespace mw
} // namespace famouso



#endif // __REALTIMESUBSCRIBEREVENTCHANNEL_H_1FC86AAEB977E9__

