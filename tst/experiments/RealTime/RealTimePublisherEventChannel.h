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
#include "math.h"

#include <string.h>


namespace famouso {
    namespace mw {
        namespace api {

            // Idee: exception handler Unterstützung als Policy (deaktivierbar)
            template <typename EC, typename Requirement,
                      template <class> class TemporalFirewall = detail::TemporalFirewallDoubleBufferedBoost>
            class RealTimePublisherEventChannel : public EC {
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


                    // logging helper
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
                     */
                    RealTimePublisherEventChannel(const Subject& subject) : EC(subject) {
                        EC::trampoline.template bind<type, &type::trampoline_impl>(this);
                        reservation_state = NoReservation;
                    }

                    /*!
                     *  \brief  Publish the event
                     *  \note   This function should be called once a period. Calling it
                     *          less will be interpreted as a time error and cause an
                     *          exception callback (NoEvent).
                     */
                    void publish(const Event & event) {
                        // TODO: check size
                        // Copy event into temporal firewall buffer
                        EventInfo & ei = tf.write_lock();
                        memcpy(ei.data, event.data, event.length);
                        ei.length = event.length;
                        timefw::Time exp = ei.expire = timefw::TimeSource::current().add_usec(period);
                        tf.write_unlock();

                        ::logging::log::emit<RT>()
                            << "publish: chan " << el::ml::LocalChanID(reinterpret_cast<uint64_t>(this))
                            << " at " << timefw::TimeSource::current() << " expiring at " << exp << "\n";

                        // Für alle Netze
                        if (reservation_state == NoReservation) {
                            ::logging::log::emit<RT>()
                                << "deliver: chan "
                                << el::ml::LocalChanID(reinterpret_cast<uint64_t>(this))
                                << " at " << timefw::TimeSource::current() << " NO RESERVATION\n";
                            signal_exception();
                        }
                    }

                    // Inplace without copy
                    //Event * get_event_buffer();
                    //void publish_event_buffer();


                    famouso::mw::api::ExceptionCallBack exception_callback;

                    // get_network_delivery_status() -> no_subscriber (alles vor reserved/reject), reserved, rejected

                    void announce() {
                        if (!exception_callback)
                            exception_callback.template bind<&ecb>();
                        EC::announce();
                    }

                    void unannounce() {
                        // TODO: für alle Netze
                        timefw::Dispatcher::instance().dequeue(deliver_task);
                    }

                private:

                    enum ReservationState {
                        NoReservation = 2,
                        Unused = 1,
                        Delivering = 0
                    };

                    // TODO: einmal für jedes Netz
                    // je netz: (slot-length), timer-handles, (RT-State)
                    uint8_t reservation_state;
                    timefw::Task deliver_task;
                    uint32_t tx_window_duration;

                    // int8_t local_deliver_net; // TODO: -1: bei publish_local bei publish, >=0 bei deliver für 1. netz mit reservierung




                    typedef typename EC::Action Action;

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
                                    deliver_task.start.set(rd->tx_ready_time);
                                    tx_window_duration = rd->tx_window_time;

                                    ::logging::log::emit<RT>()
                                        << "successfully reserved: chan "
                                        << el::ml::LocalChanID(reinterpret_cast<uint64_t>(this))
                                        << " at " << timefw::TimeSource::current() << '\n';
                                }
                            }
                            if (rd->event_type == el::ml::rt_deliv_event || rd->event_type == el::ml::rt_res_deliv_event) {
                                // Start periodic delivery of published data

                                if (reservation_state == Unused) {
                                    // Set RT status
                                    reservation_state = Delivering;

                                    // Schedule periodic deliver task
                                    deliver_task.start.set(increase_by_multiple_above(deliver_task.start.get(), static_cast<uint64_t>(period), timefw::TimeSource::current().get()));
                                    deliver_task.period = period;
                                    deliver_task.bind<type, &type::deliver>(this);
                                    deliver_task.realtime = true;
                                    timefw::Dispatcher::instance().enqueue(deliver_task);

                                    ::logging::log::emit<RT>()
                                        << "start deliver: chan "
                                        << el::ml::LocalChanID(reinterpret_cast<uint64_t>(this))
                                        << " at " << timefw::TimeSource::current() << ": first deliver at "
                                        << ::logging::log::dec << deliver_task.start << "\n";
                                }
                            } else if (rd->event_type == el::ml::rt_no_deliv_event) {
                                // Stop delivery

                                if (reservation_state == Delivering) {
                                    timefw::Dispatcher::instance().dequeue(deliver_task);
                                    reservation_state = Unused;

                                    ::logging::log::emit<RT>()
                                        << "stop deliver: chan "
                                        << el::ml::LocalChanID(reinterpret_cast<uint64_t>(this))
                                        << " at " << timefw::TimeSource::current() << "\n";
                                }
                            }
                            return 1;
                        }
                        return 0;
                    }

                    void deliver(/* NetworkID */) {
                        // publish_local bei voidNL?
                        // bei "erstem" netz publish_local

#if defined(RT_TEST_COM_LAT)
                        // Communication latency test: payload is timestamp
                        FAMOUSO_ASSERT(mel >= 8);
                        PublishParamSet pps(deliver_task.start, tx_window_duration);

                        Event e(EC::subject());
                        uint8_t buffer[mel];
                        memset(buffer, 0, mel);
                        *reinterpret_cast<uint64_t*>(buffer) = htonll(timefw::TimeSource::current().get());
                        e.length = mel;
                        e.data = buffer;
                        EC::ech().publish(*this, e, &pps);
#else
                        const EventInfo & ei = tf.read_lock();
                        // TODO: Wenn Ü-Kanal noch nicht reserviert -> Exception
                        if (timefw::TimeSource::current() < ei.expire) {
                            // expire in future
                            ::logging::log::emit<RT>()
                                << "deliver: chan "
                                << el::ml::LocalChanID(reinterpret_cast<uint64_t>(this))
                                << " at " << timefw::TimeSource::current() << " expiring at " << ei.expire << "\n";

                            /*! \todo   If the event contains a deadline attribute, also check this.
                             */
                            PublishParamSet pps(deliver_task.start, tx_window_duration);
                            Event e(EC::subject());
                            e.length = ei.length;
                            e.data = const_cast<uint8_t*>(ei.data);
                            EC::ech().publish(*this, e, &pps);
                        } else {
                            // expire in past
                            ::logging::log::emit<RT>()
                                << "deliver: chan "
                                << el::ml::LocalChanID(reinterpret_cast<uint64_t>(this))
                                << " at " << timefw::TimeSource::current() << " EXPIRED at " << ei.expire << "\n";
                            signal_exception();
                        }
                        tf.read_unlock();
#endif
                    }


                    void signal_exception() {
                        // Bei Verwendung von Temporal Firewall (komplette Flussentkopplung) in Wait-Condition, auf die von extra thread wartet
                        //exception-Handler aufrufen!
                        exception_callback();
                    }
            };


        } // namespace api
    } // namespace mw
} // namespace famouso

#endif // __REALTTIMEPUBLISHEREVENTCHANNEL_H_F5AE863048FD9B__

