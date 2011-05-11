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

#ifndef __REALTTIMEPUBLISHEREVENTCHANNEL_H_F5AE863048FD9B__
#define __REALTTIMEPUBLISHEREVENTCHANNEL_H_F5AE863048FD9B__

#include "debug.h"
#include "timefw/Dispatcher.h"

#include "mw/common/Event.h"
#include "mw/api/CallBack.h"
#include "mw/el/ml/ChannelReservationData.h"
#include "mw/el/ml/NetworkID.h"

#include "mw/attributes/Period.h"
#include "mw/attributes/ReservationState.h"
#include "mw/attributes/MaxEventLength.h"
#include "mw/attributes/detail/AttributeSetProvider.h"

#include "TemporalFirewall.h"

#include <string.h>


namespace famouso {
    namespace mw {
        namespace api {

            /*!
             *  \brief  Real time publisher event channel
             *  \tparam EC  Event channel type
             *  \tparam TemporalFirewall    Temporal firewall to use
             *  \note   Currently only supports single network configurations
             */
            template <typename EC, typename Requirement,
                      template <class> class TemporalFirewall = detail::TemporalFirewallDoubleBuffered>
            class RealTimePublisherEventChannel : public EC {
                    // Idea: exception handler support as a policy to save resources if not needed

                    typedef RealTimePublisherEventChannel type;

                protected:
                    typedef typename Requirement::template find_ct< attributes::Period<0> >::type PeriodAttrib;
                    typedef typename Requirement::template find_ct< attributes::MaxEventLength<0> >::type MELAttrib;

                    enum {
                        /// Period given in requirement attributes
                        period = PeriodAttrib::value,

                        /// Maximum event length given in requirement attributes
                        mel = MELAttrib::value
                    };

                    /// Event information struct (used in temporal firewall)
                    typedef detail::EventInfoStruct<mel> EventInfo;

                    /// Publish parameter set (used for passing real time flag and transmission window)
                    typedef typename EC::eventChannelHandler::PublishParamSet PublishParamSet;

                    /// Temporal firewall containing event information
                    TemporalFirewall<EventInfo> tf;


                    /// Logging helper
                    struct RT {
                        /*! \brief delivers the current %level of %logging */
                        static ::logging::Level::levels level() {
                            return ::logging::Level::user;
                        }
                        /*! \brief delivers the string reporting the current %level of %logging */
                        static const char * desc() {
                            return "[ RT PUB ] ";
                        }
                    };

                public:

                    /*!
                     *  \brief  Constructor
                     *  \param  subject Subject of the channel
                     */
                    RealTimePublisherEventChannel(const Subject& subject) : EC(subject) {
                        EC::trampoline.template bind<type, &type::trampoline_impl>(this);
                        reservation_state = NoReservation;
                        deliver_task.period = timefw::Time::usec(period);
                        deliver_task.realtime = true;
                        deliver_task.bind<type, &type::deliver>(this);
                    }

                    /*!
                     *  \brief  Publish the event
                     *  \note   This function should be called once a period. Calling it
                     *          less will be interpreted as a time error and cause an
                     *          exception callback
                     */
                    void publish(const Event & event) {
                        // Copy event into temporal firewall buffer
                        EventInfo & ei = tf.write_lock();
                        // TODO: check size of event
                        memcpy(ei.data, event.data, event.length);
                        ei.length = event.length;
                        timefw::Time exp = ei.expire = timefw::TimeSource::current().add(timefw::Time::usec(period));
                        tf.write_unlock();

#if (RTPEC_OUTPUT >= 2)
                        ::logging::log::emit<RT>()
                            << "publish: chan " << el::ml::getLocalChanID(this)
                            << " at " << timefw::TimeSource::current() << " expiring at " << exp << "\n";
#endif

                        if (reservation_state == NoReservation) {
#if (RTPEC_OUTPUT >= 2)
                            ::logging::log::emit<RT>()
                                << "deliver: chan "
                                << el::ml::getLocalChanID(this)
                                << " at " << timefw::TimeSource::current() << " NO RESERVATION\n";
                            signal_exception();
#endif
                        }
                    }

                    // Idea: allow inplace event initialisation to avoid copying
                    //Event * get_event_buffer();
                    //void publish_event_buffer();


                    /*!
                     *  \brief  Exception callback that is called in case the QoS
                     *          requirements cannot be met.
                     */
                    famouso::mw::api::ExceptionCallBack exception_callback;


                    /// Announce the publisher event channel
                    void announce() {
                        if (!exception_callback)
                            exception_callback.template bind<&ecb>();
                        EC::announce();
                    }

                    /// Unannounce the publisher event channel
                    void unannounce() {
                        timefw::Dispatcher::instance().dequeue(deliver_task);
                    }

                protected:

                    /// State of a network delivery channel
                    enum ReservationState {
                        /// Currently no reserved resources
                        NoReservation = 2,

                        /// Resources are reserved but there are no subscribers (no delivery)
                        Unused = 1,

                        /// Using reserved resources for event delivery
                        Delivering = 0
                    };

                    /*!
                     *  \todo   The RealTimePublisherEventChannel needs an extension to support
                     *          multi-network operation. The reservation_state, the
                     *          deliver_task and the tx_window_duration are all needed once
                     *          for each connected network.
                     */

                    /// State of the network delivery channel (ReservationState)
                    uint8_t reservation_state;

                    /// Task used for event delivery over the network
                    timefw::Task deliver_task;

                    /// Duration of the transmission window
                    timefw::Time tx_window_duration;


                    typedef typename EC::Action Action;

                    /// Implementation of the event channel trampoline delegate function
                    uint16_t trampoline_impl(Action & action) {
                        if (action.action == Action::get_requirements) {
                            // Return requirement attribute set (add real time state attrib)
                            // \todo    To support real time on multiple networks per
                            //          node, we need to transmit multiple reservation
                            //          states per publisher, e.g. we need support for
                            //          multiple instances of the same attribute (and
                            //          iterator access)
                            typedef typename attributes::detail::SortedAttributeSequence<
                                        typename boost::mpl::push_back<
                                            typename Requirement::sequence,
                                            attributes::ReservationState
                                        >::type
                                    >::result sorted_sequence;
                            typedef attributes::AttributeSet<sorted_sequence> AttrSet;
                            if (action.buffer) {
                                AttrSet * attrib = new (action.buffer) AttrSet;
                                attributes::ReservationState * rt =
                                        attrib->template find_rt< attributes::ReservationState >();
                                rt->setValue(reservation_state);
                            }
                            return AttrSet::overallSize;
                        }
                        if (action.action == Action::set_real_time_reserv_state) {
                            const el::ml::ChannelReservationData * rd = reinterpret_cast<const el::ml::ChannelReservationData *>(action.buffer);

                            //if (rd->event_type == el::ml::rt_res_event || rd->event_type == el::ml::rt_res_deliv_event) {
                            if (rd->event_type <= el::ml::rt_res_deliv_event) {
                                // TX channel successfully reserved

                                if (reservation_state == NoReservation) {
                                    // Set RT status
                                    reservation_state = Unused;

                                    // Save tx channel params
                                    deliver_task.start.set_usec(rd->tx_ready_time);
                                    tx_window_duration = timefw::Time::usec(rd->tx_window_time);

#if (RTPEC_OUTPUT >= 1)
                                    ::logging::log::emit<RT>()
                                        << "successfully reserved: chan "
                                        << el::ml::getLocalChanID(this)
                                        << " at " << timefw::TimeSource::current() << '\n';
#endif
                                }
                            }
                            if (rd->event_type == el::ml::rt_deliv_event || rd->event_type == el::ml::rt_res_deliv_event) {
                                // Start periodic delivery of published data

                                if (reservation_state == Unused) {
                                    // Set RT status
                                    reservation_state = Delivering;

                                    // Schedule periodic deliver task
                                    deliver_task.start.futurify(timefw::Time::usec(period), timefw::TimeSource::current());
                                    timefw::Dispatcher::instance().enqueue(deliver_task);

#if (RTPEC_OUTPUT >= 1)
                                    ::logging::log::emit<RT>()
                                        << "start deliver: chan "
                                        << el::ml::getLocalChanID(this)
                                        << " at " << timefw::TimeSource::current() << ": first deliver at "
                                        << ::logging::log::dec << deliver_task.start << "\n";
#endif
                                }
                            } else if (rd->event_type == el::ml::rt_no_deliv_event) {
                                // Stop delivery

                                if (reservation_state == Delivering) {
                                    timefw::Dispatcher::instance().dequeue(deliver_task);
                                    reservation_state = Unused;

#if (RTPEC_OUTPUT >= 1)
                                    ::logging::log::emit<RT>()
                                        << "stop deliver: chan "
                                        << el::ml::getLocalChanID(this)
                                        << " at " << timefw::TimeSource::current() << "\n";
#endif
                                }
                            }
                            return 1;
                        }
                        return 0;
                    }

                    /// Delivers the event \p e over the network
                    void deliver_to_net(const Event & e/*, NetworkID */) {
                        PublishParamSet pps(deliver_task.start, tx_window_duration);
                        EC::ech().publish(*this, e, &pps);
                    }

                    /// Delivers the current event (read from temporal firewall)
                    void deliver(/* NetworkID */) {
                        /*!
                         *  \todo   To support multi- and no-network operation the deliver*
                         *          functions of the RealTimePublisherEventChannel has to
                         *          modified to ensure that each event is published locally
                         *          only and exactly once in all configurations.
                         */
                        const EventInfo & ei = tf.read_lock();
                        if (timefw::TimeSource::current() < ei.expire) {
                            // expire in future
#if (RTPEC_OUTPUT >= 2)
                            ::logging::log::emit<RT>()
                                << "deliver: chan "
                                << el::ml::getLocalChanID(this)
                                << " at " << timefw::TimeSource::current() << " expiring at " << ei.expire << "\n";
#endif

                            /*! \todo   If the event contains a deadline attribute,
                             *          also check this. (RealTimePublisherEventChannel::deliver())
                             */
                            Event e(EC::subject());
                            e.length = ei.length;
                            e.data = const_cast<uint8_t*>(ei.data);
                            deliver_to_net(e);
                        } else {
                            // expire in past
#if (RTPEC_OUTPUT >= 2)
                            ::logging::log::emit<RT>()
                                << "deliver: chan "
                                << el::ml::getLocalChanID(this)
                                << " at " << timefw::TimeSource::current() << " EXPIRED at " << ei.expire << "\n";
                            signal_exception();
#endif
                        }
                        tf.read_unlock();
                    }

                    /// Signal an exception to the application
                    void signal_exception() {
                        // This could by modified to post the exception for deferred
                        // execution (for full flow decoupling).
                        exception_callback();
                    }
            };

        } // namespace api
    } // namespace mw
} // namespace famouso

#endif // __REALTTIMEPUBLISHEREVENTCHANNEL_H_F5AE863048FD9B__

