/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __NETWORKSCHEDULE_H_B01D3EA12111F1__
#define __NETWORKSCHEDULE_H_B01D3EA12111F1__

#include <stdint.h>
#include "timefw/Time.h"

#include "mw/el/ml/NetworkID.h"
#include "detail/RealTimeTxChannel.h"
#include "detail/Subscriber.h"
#include "detail/KnownPubSubList.h"
#include "detail/NetworkTimingConfig.h"
#include "detail/SlotScheduler.h"
#include "detail/RTNetSchedulerBase.h"
#include "detail/value_types.h"

namespace famouso {
    namespace mw {
        namespace rt_net_sched {


            class NetworkSchedule {

                    el::ml::NetworkID network_id;

                    const NetworkTimingConfig network_timing;

                    typedef KnownPubSubList<RealTimeTxChannel> RealTimeAnnouncements;

                    RealTimeAnnouncements rt_announcements;

                    typedef KnownPubSubList<Subscriber> Subscriptions;

                    Subscriptions subscriptions;

                    SlotScheduler slot_scheduler;

                    /// Duration of the TDMA cycle (in microseconds)
                    uint64_t cycle_duration_us;

                    /// Starting time of the last TDMA cycle introducing a duration change (in microseconds)
                    uint64_t cycle_start_us;

                    uint64_t get_next_cycle_start() {
                        uint64_t curr = timefw::TimeSource::current().get();
                        return increase_by_multiple_above(cycle_start_us, cycle_duration_us, curr);
                    }

                    void set_cycle_duration(uint64_t duration) {
                        if (cycle_duration_us == duration)
                            return;
                        cycle_start_us = get_next_cycle_start();
                        cycle_duration_us = duration;
                    }

                    RTNetSchedulerBase & scheduler;

                    void reserv(RealTimeTxChannel & rt_pub) {
                        // Try to reserve communication channel
                        unsigned int aslot_shift = 0;
                        int aslot_min = -1, aslot_width = -1;

                        if (rt_pub.slot_bounds_given) {
                            aslot_min = rt_pub.slot_aslot.shift_min;
                            aslot_width = rt_pub.slot_aslot.shift_width;
                        }

                        ::logging::log::emit() << "Trying to reserve real time network resources... ";

                        if (slot_scheduler.reserve(rt_pub.slot_aslot.period, rt_pub.slot_aslot.length, aslot_min, aslot_width, aslot_shift)) {
                            ::logging::log::emit() << "success!" << ::logging::log::endl;
                            set_cycle_duration(slot_scheduler.cycle_length() * network_timing.plan_granul_us);
                            rt_pub.init_reserv(aslot_shift, network_timing);

                            // TODO: if neccessary, send free_slots to BE-Poller and wait for ACK before sending reservation (only once per announcements msg?)
                            // wait for subscribers?

                            bool deliver = check_subscriber(rt_pub.subject, rt_pub.network_id);
                            if (deliver)
                                rt_pub.status = RealTimeTxChannel::delivering;
                            else
                                rt_pub.status = RealTimeTxChannel::waiting_for_subscriber;

                            // Send reservation for this channel
                            scheduler.publish_reservation(rt_pub.node_id, rt_pub.lc_id, rt_pub.network_id,
                                                          get_next_cycle_start() + rt_pub.slot_usec.shift,
                                                          rt_pub.slot_usec.tx_window,
                                                          deliver);
                        } else {
                            ::logging::log::emit() << "failure!" << ::logging::log::endl;
                        }
                        slot_scheduler.log_free_list();
                        ::logging::log::emit() << "Real time announcements\n";
                        rt_announcements.log();
                    }

                    /// Return whether there is a subscriber of subject in network
                    bool check_subscriber(const Subject & subject, const el::ml::NetworkID & network_id) {
                        Subscriptions::iterator it = subscriptions.begin();
                        while (it != subscriptions.end()) {
                            Subscriber & s = subscriptions.get(it);
                            if (s.subject == subject && s.network_id == network_id) {
                                return true;
                            }
                            ++it;
                        }
                        return false;
                    }

                    bool unannounce(const RealTimeTxChannel & pub) {
                        ::logging::log::emit() << "Unannouncement of real time channel " << pub.subject
                                               << ::logging::log::endl;
                        slot_scheduler.free(pub.slot_aslot.period, pub.slot_aslot.length, pub.slot_aslot.shift);
                        slot_scheduler.log_free_list();
                        // TODO: check if a wait_for_resservation channel can be scheduled now (deferred)
                        return true;
                    }

                    bool unsubscribe(const Subscriber & sub) {
                        ::logging::log::emit() << "Unsubscription of channel " << sub.subject
                                               << ::logging::log::endl;
                        // TODO: check if there are more subscribers of this subject and change publisher state if neccessary
                        return true;
                    }

                public:
                    template <class RTNetScheduler>
                    NetworkSchedule(const el::ml::NetworkID & network_id, const NetworkTimingConfig & ntc, RTNetScheduler & sched) :
                            network_id(network_id), network_timing(ntc),
                            cycle_duration_us(ntc.plan_granul_us), cycle_start_us(0),
                            scheduler(sched) {
                        sched.register_network(this);
                    }

                    const el::ml::NetworkID & get_network_id() const {
                        return network_id;
                    }



                    void begin_announcements_processing(const NodeID & node_id) {
                        rt_announcements.begin_update(node_id);
                    }

                    void process_announcement(const NodeID & node_id, const el::ml::LocalChanID & lc_id,
                                              const el::ml::NetworkID & network_id, const Subject & subject,
                                              res_state_t reserv_state, period_t period, mel_t max_event_length,
                                              repetition_t repetition,
                                              uint64_t * slot_bound_start, uint64_t * slot_bound_end) {

                        // Assumes no changes in known announcements... returns only new publisher (uninitialized)
                        bool newly_announced;
                        RealTimeTxChannel & rt_pub = rt_announcements.update(node_id, lc_id, newly_announced);

                        if (!newly_announced) {
                            // TODO: match states and init retransmission if necessary
                        } else {
                            // Newly announced publisher: reserve tx channel
                            rt_pub.init(node_id, lc_id, network_id, subject, period,
                                        max_event_length, repetition,
                                        slot_bound_start, slot_bound_end,
                                        get_next_cycle_start(),
                                        network_timing);
                            reserv(rt_pub);
                        }
                    }

                    void end_announcements_processing() {
                        RealTimeAnnouncements::UnannounceCallback ucb;
                        ucb.bind<NetworkSchedule, &NetworkSchedule::unannounce>(this);
                        rt_announcements.end_update(ucb);
                    }


                    void begin_subscriptions_processing(const NodeID & node_id) {
                        subscriptions.begin_update(node_id);
                    }

                    void process_subscription(const NodeID & node_id, const el::ml::LocalChanID & lc_id, const el::ml::NetworkID & network_id, const Subject & subject) {
                        bool newly_inserted;
                        Subscriber & s = subscriptions.update(node_id, lc_id, newly_inserted);
                        if (newly_inserted) {
                            // New subscriber
                            s.init(node_id, lc_id, network_id, subject);

                            // Try to find producer channels for this suscriber
                            RealTimeAnnouncements::iterator it = rt_announcements.begin();
                            while (it != rt_announcements.end()) {
                                RealTimeTxChannel & s = rt_announcements.get(it);
                                if (s.subject == subject && s.network_id == network_id && s.status == RealTimeTxChannel::waiting_for_subscriber) {
                                    // Start delivery of events
                                    scheduler.publish_deliv(s.node_id, s.lc_id, s.network_id);
                                }
                                ++it;
                            }
                        }
                    }

                    void end_subscriptions_processing() {
                        Subscriptions::UnannounceCallback ucb;
                        ucb.bind<NetworkSchedule, &NetworkSchedule::unsubscribe>(this);
                        subscriptions.end_update(ucb);
                    }
            };

        } // namespace rt_net_sched
    } // namespace mw
} // namespace famouso

#endif // __NETWORKSCHEDULE_H_B01D3EA12111F1__

