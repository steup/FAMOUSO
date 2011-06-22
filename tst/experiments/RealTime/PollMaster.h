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

#ifndef __POLLMASTER_H_176A970BC54ADC__
#define __POLLMASTER_H_176A970BC54ADC__

#include "debug.h"
#include "mw/common/Event.h"

#include "mw/attributes/Period.h"
#include "mw/attributes/MaxEventLength.h"
#include "mw/attributes/ReservationState.h"
#include "mw/attributes/AttributeSet.h"

#include "mw/el/ml/ChannelRequirementsEvent.h"
#include "mw/el/ml/RealTimeSetResStateEvent.h"

#include "detail/value_types.h"

#include <list>

//#include "mw/afp/shared/hexdump.h"
//using famouso::mw::afp::shared::hexdump;

namespace famouso {
    namespace mw {
        namespace poll_master {

            typedef famouso::mw::afp::defrag::detail::Bit Bit;
            typedef famouso::mw::afp::defrag::detail::BitArray<0> BitArray;

            class PollMasterSchedule : public BitArray {
                    uint64_t get_useable_aslot_count(uint64_t start) {
                        uint64_t count = 0;
                        for (uint64_t i = start; i < size(); ++i) {
                            if (operator[](i).get() == false)
                                return count;
                            ++count;
                        }
                        for (uint64_t i = 0; i < start; ++i) {
                            if (operator[](i).get() == false)
                                return count;
                            ++count;
                        }
                        return count;
                    }
                public:

                    /*!
                     *  \brief  Find usable aslot
                     *  \param  start   Input: where to start search,
                     *                  Output: first usable aslot
                     *  \param  count   Output: count of usable aslots
                     */
                    void find_next_usable_aslot(uint64_t & start, uint64_t & count) {
                        start %= size();
                        for (uint64_t i = start; i < size(); ++i) {
                            count = get_useable_aslot_count(i);
                            if (count) {
                                start = i;
                                return;
                            }
                        }
                        for (uint64_t i = 0; i < start; ++i) {
                            count = get_useable_aslot_count(i);
                            if (count) {
                                start = i;
                                return;
                            }
                        }
                        ::logging::log::emit() << "No Media time left for polling\n";
                        exit(1);
                    }
            };

            /*!
             *  \brief  Real time poll master
             *  \tparam PEC Best-Effort PublisherEventChannel type
             *  \tparam SEC Best-Effort SubscriberEventChannel type
             *
             *  ATM the poll master does not split large free slots
             *  to poll multiple nodes, but only polls one node per
             *  slot.
             */
            template <class PEC, class SEC>
            class PollMaster : public std::list<NodeID> {

                    typedef PollMaster ThisType;
                    typedef std::list<NodeID> NodeList;

                    PEC man_chan_pub;
                    SEC man_chan_sub;

                    // should not be forwarded through gateways
                    PEC poll_chan_pub;

                    uint64_t us_per_aslot;
                    double overhead_us;
                    uint64_t min_poll_us;

                    PollMasterSchedule usable_aslots;

                    /// Cycle synced after receiving first PollCycle-Event (owning full cycle if false)
                    bool cycle_synced;
                    uint64_t last_cycle_start_us;

                    uint64_t next_usable_aslot_start;
                    uint64_t next_usable_aslot_count;

                    NodeList::iterator next_node;
                    int32_t next_tx_window;

                    /// A cycle change invalidates next* variables ATM
                    bool cycle_updated;

                    timefw::Task poll_task;

                public:
                    PollMaster(uint64_t us_per_aslot_, uint64_t overhead_us_, uint64_t min_poll_us_) :
                        man_chan_pub("ManChan!"),
                        man_chan_sub("ManChan!"),
                        poll_chan_pub("NRTPoll!"),
                        us_per_aslot(us_per_aslot_),
                        overhead_us(overhead_us_),
                        min_poll_us(min_poll_us_),
                        cycle_synced(false),
                        last_cycle_start_us(0),
                        next_usable_aslot_start(0),
                        next_usable_aslot_count(1),
                        next_node(NodeList::begin()),
                        next_tx_window(0),
                        cycle_updated(false)
                    {
                        man_chan_sub.callback.template bind<
                                                ThisType,
                                                &ThisType::process_poll_cycle_event>(this);
                        man_chan_sub.subscribe();
                        man_chan_pub.announce();
                        poll_chan_pub.announce();
                        poll_task.template bind<ThisType,
                                                &ThisType::poll_task_func>(this);
                        prepare_poll();
                    }


                protected:
                    int64_t get_tx_window_duration(uint64_t us_to_use) {
                        double drift = 1 + 1e-4;
                        return ((double)us_to_use - overhead_us) / drift;
                    }

                    void prepare_poll() {
                        // Prepare next poll
                        if (!cycle_synced) {
                            // No real time channels reserved and used -> owning all time
                            next_tx_window = get_tx_window_duration(min_poll_us);

                            // Schedule task for next poll
                            poll_task.start.futurify(
                                            timefw::Time::usec(min_poll_us),
                                            timefw::TimeSource::current());

                        } else {
                            // Real time channels in use

                            // Find next usable slot
                            uint64_t stop = next_usable_aslot_start + usable_aslots.size();
                            uint64_t us_to_use;
                            do {
                                next_usable_aslot_start += next_usable_aslot_count;
                                usable_aslots.find_next_usable_aslot(next_usable_aslot_start, next_usable_aslot_count);
                                us_to_use = next_usable_aslot_count * us_per_aslot;
                                next_tx_window = get_tx_window_duration(us_to_use);
                            } while (next_tx_window < 0 &&
                                     us_to_use >= min_poll_us &&
                                     next_usable_aslot_start <= stop);
                            // If this fails, there are no adequate slots for NRT
                            FAMOUSO_ASSERT(next_usable_aslot_start <= stop);

                            // Schedule task for next poll
                            poll_task.start.set_usec(
                                    // Problem: skip current cycle FIXME
                                    increase_by_multiple_above(               // Last cycle start +
                                            last_cycle_start_us,
                                            usable_aslots.size() * us_per_aslot,
                                            timefw::TimeSource::current().get_usec()) +
                                    next_usable_aslot_start * us_per_aslot);  // shift inside cycle
                        }
                        timefw::Dispatcher::instance().enqueue(poll_task);
                    }

                    void poll_task_func() {
                        // PollCycle-Event changes poll cycle -> maybe
                        // next_tx_window is not usable anymore
                        // TODO: a more efficient solution than skipping a poll
                        if (!cycle_updated) {
                            if (next_node == end())
                                next_node = begin();
                            if (next_node != end()) {
                                // Publish poll event
                                publish_poll(*next_node, next_tx_window);
                                ++next_node;
                            } else {
                                ::logging::log::emit()
                                    << "[NRT-Poll] No node to poll!\n";
                            }
                        }
                        cycle_updated = false;

                        // Schedule next poll event
                        prepare_poll();
                    }

                    void change_cycle() {
                        last_cycle_start_us = increase_by_multiple_above(
                                                    last_cycle_start_us,
                                                    usable_aslots.size() * us_per_aslot,
                                                    timefw::TimeSource::current().get_usec());
                        cycle_synced = true;
                        cycle_updated = true;
                    }

                    void process_poll_cycle_event(const Event & event) {
                        uint8_t event_type = el::ml::get_event_type(event.data, event.length);
                        if (event_type == el::ml::poll_cycle_event) {
                            uint8_t * ed = event.data;
                            uint16_t el = event.length;
                            ++ed; --el;

                            // TODO: add network ID

                            last_cycle_start_us = ntohll(*reinterpret_cast<uint64_t*>(ed));
                            ed += sizeof(last_cycle_start_us); el -= sizeof(last_cycle_start_us);

                            uint32_t bit_count = ntohl(*reinterpret_cast<uint32_t*>(ed));
                            ed += sizeof(bit_count); el -= sizeof(bit_count);
                            usable_aslots.resize(bit_count);

                            ::logging::log::emit()
                                << "[NRT-Poll] Poll cycle event: last_cycle_start_us "
                                << ::logging::log::dec << last_cycle_start_us
                                << ", bit_count " << bit_count
                                << "\n";

                            FAMOUSO_ASSERT(el * 8 >= bit_count);
                            memcpy(usable_aslots.get_buffer(), ed, el);

                            change_cycle();

                            // TODO: send ACK
                        }
                    }

                    void publish_poll(const NodeID & node_id, uint32_t tx_window_time) {
                        ::logging::log::emit()
                            << "[NRT-Poll] Polling node " << node_id
                            << " at " << timefw::TimeSource::current()
                            << " for " << tx_window_time << " us\n";
                        uint16_t len = sizeof(NodeID) + sizeof(uint32_t);
                        uint8_t buffer[len];
                        *reinterpret_cast<NodeID *>(buffer) = node_id;
                        *reinterpret_cast<uint32_t *>(buffer + sizeof(NodeID)) = htonl(tx_window_time);

                        Event e(poll_chan_pub.subject());
                        e.data = buffer;
                        e.length = len;
                        poll_chan_pub.publish(e);
                    }

            };


        } // namespace rt_net_sched
    } // namespace mw
} // namespace famouso

#endif // __POLLMASTER_H_176A970BC54ADC__

