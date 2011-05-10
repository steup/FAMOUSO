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

            template <typename SEC, typename Requirement>
            class RealTimeSubscriberEventChannelBase : public SEC {
                    typedef RealTimeSubscriberEventChannelBase type;

                protected:
                    typedef typename Requirement::template find_ct< attributes::Period<0> >::type PeriodAttrib;
                    typedef typename Requirement::template find_ct< attributes::MaxEventLength<0> >::type MELAttrib;

                    enum {
                        /// Period given in requirement attributes
                        period = PeriodAttrib::value,

                        /// Maximum event length given in requirement attributes
                        mel = MELAttrib::value
                    };

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
                    RealTimeSubscriberEventChannelBase(const Subject& subject) : SEC(subject) {
                        SEC::trampoline.template bind<type, &type::trampoline_impl>(this);
                    }

                private:

                    typedef typename SEC::Action Action;

                    uint16_t trampoline_impl(Action & action) {
                        if (action.action == Action::get_requirements) {
                            // Return requirement attribute set
                            if (action.buffer) {
                                new (action.buffer) Requirement;
                            }
                            return Requirement::overallSize;
                        }
                        return 0;
                    }
            };


            template <typename SEC, typename Requirement,
                      template <class> class TemporalFirewall = detail::TemporalFirewallDoubleBuffered>
            class RealTimeSubscriberEventChannel : public RealTimeSubscriberEventChannelBase<SEC, Requirement> {
                    typedef RealTimeSubscriberEventChannel type;
                    typedef RealTimeSubscriberEventChannelBase<SEC, Requirement> Base;

                protected:

                    /// Event information struct
                    typedef detail::EventInfoStruct<Base::mel> EventInfo;

                    /// Temporal firewall containing event information
                    TemporalFirewall<EventInfo> tf;

                    timefw::Task subscriber_task;

                    // Middleware notify callback writing temporal firewall
                    void callback_periodic_notify_impl(const Event & event) {
                        // TODO: Deadline prüfen, Filter anwenden, Strength prüfen (z.B. previous_strength als member-var zwischenspeichern)...
                        if (event.length > Base::mel)
                            return;
                        EventInfo & ei = tf.write_lock();
                        memcpy(ei.data, event.data, event.length);
                        ei.length = event.length;
                        ei.expire = timefw::TimeSource::current().add(timefw::Time::usec(Base::period)); // oder deadline, TODO: omission beachten
                        tf.write_unlock();
                    }

                    // may run in another thread
                    void subscriber_task_func() {
                        const EventInfo & ei = tf.read_lock();
#if 0
                        // More stable concerning time errors
                        if (timefw::TimeSource::current() < ei.expire) {
#else
                        // Less end-to-end latency jitter
                        if (subscriber_task.start < ei.expire) {
#endif
                            // expire in future
#if (RTSEC_OUTPUT >= 2)
                            ::logging::log::emit<typename Base::RT>()
                                << "update notify: chan "
                                << el::ml::getLocalChanID(this)
                                << " at " << timefw::TimeSource::current() << " expiring at " << ei.expire << "\n";
#endif
                            if (notify_callback) {
                                Event e(SEC::subject());
                                e.data = const_cast<uint8_t*>(ei.data);
                                e.length = ei.length;
                                notify_callback(e);
                            }
                        } else {
                            // expire in past
#if (RTSEC_OUTPUT >= 2)
                            ::logging::log::emit<typename Base::RT>()
                                << "exception notify: chan "
                                << el::ml::getLocalChanID(this)
                                << " at " << timefw::TimeSource::current() << " EXPIRED at " << ei.expire << "\n";
#endif
                            if (exception_callback) {
                                exception_callback();
                            }
                        }
                        tf.read_unlock();
                    }

                public:
                    RealTimeSubscriberEventChannel(const famouso::mw::Subject & subj,
                                                   const timefw::Time & sub_task_start = timefw::TimeSource::current()) :
                        Base(subj),
                        subscriber_task(sub_task_start, timefw::Time::usec(Base::period), true)
                    {
                        SEC::callback.template bind<type, &type::callback_periodic_notify_impl>(this);
                        subscriber_task.template bind<type, &type::subscriber_task_func>(this);
                        timefw::Dispatcher::instance().enqueue(subscriber_task);
#if (RTSEC_OUTPUT >= 1)
                        ::logging::log::emit<typename Base::RT>()
                            << "start subscriber task: chan "
                            << el::ml::getLocalChanID(this)
                            << " at " << subscriber_task.start << '\n';
#endif
                    }

                    /*!
                     *  \brief  
                     *  bei Subscriber anstelle des notify aufgerufen, wenn kein Event gemäß Spezifikation (Subject, Filter, QoS-Anforderungen) erhalten
                     */
                    famouso::mw::api::ExceptionCallBack exception_callback;
                    famouso::mw::api::SECCallBack notify_callback;
            };

        } // namespace api
    } // namespace mw
} // namespace famouso



#endif // __REALTIMESUBSCRIBEREVENTCHANNEL_H_1FC86AAEB977E9__

