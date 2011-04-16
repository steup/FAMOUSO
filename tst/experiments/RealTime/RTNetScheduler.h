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

#ifndef __RTNETSCHEDULER_H_C58F6B00B76072__
#define __RTNETSCHEDULER_H_C58F6B00B76072__

#include "debug.h"
#include "mw/common/Event.h"

#include "mw/attributes/Period.h"
#include "mw/attributes/MaxEventLength.h"
#include "mw/attributes/ReservationState.h"
#include "mw/attributes/AttributeSet.h"

#include "mw/el/ml/ChannelRequirementsEvent.h"
#include "mw/el/ml/RealTimeSetResStateEvent.h"

#include "NetworkSchedule.h"
#include "detail/RTNetSchedulerBase.h"
#include "detail/value_types.h"

#include <map>

//#include "mw/afp/shared/hexdump.h"
//using famouso::mw::afp::shared::hexdump;

namespace famouso {
    namespace mw {
        namespace rt_net_sched {

            // PEC, SEC: BE-Channel
            /*!
             *  \brief  Real time network scheduler
             *  \tparam PEC Best-Effort PublisherEventChannel type
             *  \tparam SEC Best-Effort SubscriberEventChannel type
             *
             *  Supports scheduling multiple subnetworks.
             */
            template <class PEC, class SEC>
            class RTNetScheduler : public RTNetSchedulerBase {

                    typedef RTNetScheduler ThisType;

                    typedef attributes::Period<0> PeriodType;
                    typedef attributes::MaxEventLength<0> MaxEventLengthType;
                    typedef attributes::ReservationState ReservationStateType;
                    typedef attributes::AttributeSet<> AttrSet;

                    PEC man_chan_pub;
                    SEC man_chan_sub;

                    typedef std::map<el::ml::NetworkID, NetworkSchedule *> NetworkContainer;
                    NetworkContainer networks;

                    NetworkSchedule * get_network(const el::ml::NetworkID & network_id) {
                        NetworkContainer::iterator it = networks.find(network_id);
                        if (it == networks.end())
                            return 0;
                        else
                            return it->second;
                    }

                    /// Add \p network_schedule to set of network schedules
                    void register_network(NetworkSchedule * network_schedule) {
                        networks[network_schedule->get_network_id()] = network_schedule;
                    }

                    /// Allow NetworkSchedule to call register_network()
                    friend class NetworkSchedule;

                    void publish_reservation(const NodeID & node_id, const el::ml::LocalChanID & lc_id, const el::ml::NetworkID & network_id,
                                             uint64_t tx_ready_time, uint64_t tx_window_time,
                                             bool deliver) {
                        ::logging::log::emit()
                            << "Publishing " << (deliver ? "ResDeliv" : "Res")
                            << " to node " << node_id << ", lc_id " << lc_id
                            << ", network_id " << network_id
                            << " with tx_ready " << timefw::Time(tx_ready_time)
                            << ", tx_window " << timefw::Time(tx_window_time) << '\n';
                        uint16_t len = el::ml::RealTimeSetResStateEvent::size(true);
                        uint8_t buffer[len];
                        el::ml::RealTimeSetResStateEvent rw(buffer, len);
                        rw.write_head(deliver ? el::ml::rt_res_deliv_event : el::ml::rt_res_event, node_id);
                        rw.write_reserv_tail(lc_id, network_id, tx_ready_time, tx_window_time);

                        Event e(man_chan_pub.subject());
                        e.data = buffer;
                        e.length = len;
                        man_chan_pub.publish(e);
                    }

                    void publish_deliv(const NodeID & node_id, const el::ml::LocalChanID & lc_id, const el::ml::NetworkID & network_id) {
                        ::logging::log::emit()
                            << "Publishing Deliv to node " << node_id << ", lc_id " << lc_id
                            << ", network_id " << network_id << ::logging::log::endl;
                        uint16_t len = el::ml::RealTimeSetResStateEvent::size(false);
                        uint8_t buffer[len];
                        el::ml::RealTimeSetResStateEvent rw(buffer, len);
                        rw.write_head(el::ml::rt_deliv_event, node_id);
                        rw.write_no_reserv_tail(lc_id, network_id);

                        Event e(man_chan_pub.subject());
                        e.data = buffer;
                        e.length = len;
                        man_chan_pub.publish(e);
                    }

                    void publish_no_deliv(const NodeID & node_id, const el::ml::LocalChanID & lc_id, const el::ml::NetworkID & network_id) {
                        ::logging::log::emit()
                            << "Publishing NoDeliv to node " << node_id << ", lc_id " << lc_id
                            << ", network_id " << network_id << ::logging::log::endl;
                        uint16_t len = el::ml::RealTimeSetResStateEvent::size(false);
                        uint8_t buffer[len];
                        el::ml::RealTimeSetResStateEvent rw(buffer, len);
                        rw.write_head(el::ml::rt_no_deliv_event, node_id);
                        rw.write_no_reserv_tail(lc_id, network_id);

                        Event e(man_chan_pub.subject());
                        e.data = buffer;
                        e.length = len;
                        man_chan_pub.publish(e);
                    }

                    void print_pub_info(const Subject & subject, const el::ml::LocalChanID & lc_id, const el::ml::NetworkID & network_id, const AttrSet * req_attr) {
                        ::logging::log::emit() << "\tpublisher : subject " << subject
                                               << ", lc_id " << lc_id
                                               << ", network_id " << network_id
                                               << ", Attribute:"
                                               << ::logging::log::endl;
                        //hexdump((uint8_t*)req_attr, req_attr->length());

                        const PeriodType * period_p = req_attr->template find_rt<PeriodType>();
                        if (period_p) {
                            period_t period = period_p->getValue();
                            ::logging::log::emit() << "\t\tperiod " << ::logging::log::dec
                                                   << period
                                                   << ::logging::log::endl;
                        }

                        const MaxEventLengthType * mes_p = req_attr->template find_rt<MaxEventLengthType>();
                        if (mes_p) {
                            mel_t max_event_length = mes_p->getValue();
                            ::logging::log::emit() << "\t\tmax event length " << ::logging::log::dec
                                                   << max_event_length
                                                   << ::logging::log::endl;
                        }

                        const ReservationStateType * rt_p = req_attr->template find_rt<ReservationStateType>();
                        if (rt_p) {
                            uint16_t rt_state = rt_p->getValue();
                            ::logging::log::emit() << "\t\tRealTime state " << ::logging::log::dec << rt_state
                                                   << ::logging::log::endl;
                        }
                    }

                    void process_announcements(const Event & event) {
                        el::ml::ChannelRequirementsReader crr(event.data, event.length);

                        uint8_t msg_type;
                        NodeID node_id;
                        el::ml::NetworkID network_id;
                        crr.read_head(msg_type, node_id, network_id);

                        ::logging::log::emit() << "Announcements from node " << node_id
                                               << ::logging::log::endl;
                        //hexdump(event.data, event.length);

                        NetworkSchedule * net = get_network(network_id);
                        if (!net) return;

                        // Starts update of real time publisher list
                        net->begin_announcements_processing(node_id);

                        // TODO: multi-Net je knoten
                        while (crr.further_channel()) {
                            Subject subject; el::ml::LocalChanID lc_id; const AttrSet * req_attr;
                            crr.read_channel(subject, lc_id, req_attr);

                            print_pub_info(subject, lc_id, network_id, req_attr);

                            // Extract attributes
                            const ReservationStateType * rt_p = req_attr->template find_rt<const ReservationStateType>();
                            if (rt_p) {
                                // Real Time announcement
                                const PeriodType * period_p = req_attr->template find_rt<PeriodType>();
                                const MaxEventLengthType * mes_p = req_attr->template find_rt<MaxEventLengthType>();

                                if (period_p && mes_p) {
                                    // New real time announcement
                                    period_t period_us = period_p->getValue();
                                    mel_t max_event_length_bytes = mes_p->getValue();

                                    net->process_announcement(node_id, lc_id, network_id, subject,
                                                              rt_p->getValue(), period_us, max_event_length_bytes, 0);

                                } else {
                                    ::logging::log::emit() << "Essential real time attribute missing: ignoring announcement"
                                                           << ::logging::log::endl;
                                }
                            }
                        }

                        net->end_announcements_processing();
                    }

                    void process_subscriptions(const Event & event) {
                        el::ml::ChannelRequirementsReader crr(event.data, event.length);

                        uint8_t msg_type;
                        NodeID node_id;
                        el::ml::NetworkID network_id;
                        crr.read_head(msg_type, node_id, network_id);

                        ::logging::log::emit() << "Subscriptions from node " << node_id
                                               << ::logging::log::endl;

                        NetworkSchedule * net = get_network(network_id);
                        if (!net) return;

                        // Starts update of subscriber list
                        net->begin_subscriptions_processing(node_id);

                        // TODO: multi-Net
                        while (crr.further_channel()) {
                            Subject subject; el::ml::LocalChanID lc_id; const AttrSet * req_attr;
                            crr.read_channel(subject, lc_id, req_attr);

                            ::logging::log::emit() << "\tsubscriber : subject " << subject
                                                   << ", lc_id " << lc_id
                                                   << ", network_id " << ::logging::log::hex << network_id
                                                   << ::logging::log::endl;
                            net->process_subscription(node_id, lc_id, network_id, subject);
                        }

                        net->end_subscriptions_processing();
                    }

                    void incoming_management_event(const Event & event) {

                        uint8_t event_type = el::ml::get_event_type(event.data, event.length);
                        if (event_type == el::ml::announcements_event)
                            process_announcements(event);
                        else if (event_type == el::ml::subscriptions_event)
                            process_subscriptions(event);

                        return;
                    }


                public:

                    RTNetScheduler() :
                        man_chan_pub("ManChan!"),
                        man_chan_sub("ManChan!")
                    {
                        man_chan_sub.callback.template bind<ThisType, & ThisType::incoming_management_event>(this);
                        man_chan_sub.subscribe();
                        man_chan_pub.announce();
                    }

            };


        } // namespace rt_net_sched
    } // namespace mw
} // namespace famouso

#endif // __RTNETSCHEDULER_H_C58F6B00B76072__

