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

#ifndef __REALTIMETXCHANNEL_H_3A33121E118371__
#define __REALTIMETXCHANNEL_H_3A33121E118371__

#include <stdint.h>

#include "mw/common/Subject.h"
#include "mw/common/NodeID.h"
#include "mw/el/ml/LocalChanID.h"
#include "mw/el/ml/NetworkID.h"

#include "NetworkTimingConfig.h"
#include "value_types.h"
#include "util/math.h"


namespace famouso {
    namespace mw {
        namespace rt_net_sched {

            struct RealTimeTxChannel {
                enum Status {
                    waiting_for_reservation,
                    waiting_for_subscriber,
                    delivering,
                    waiting_for_poll_master_ack,
                    waiting_for_producer_ack
                };

                Status status;
                NodeID node_id;

                /// Local channel ID
                el::ml::LocalChanID lc_id;

                /// Network ID
                el::ml::NetworkID network_id;

                /// Subject, only needed for verbose logging
                Subject subject;

                // Whetehr slot_usec.shift_* and slot_aslot.shift_* are valid
                bool slot_bounds_given;

                struct SlotInfoUSec {
                    uint64_t period;
                    uint64_t length;
                    uint64_t tx_window;       // relative to shift
                    uint64_t shift;
                    uint64_t shift_min;
                    uint64_t shift_max;
                } slot_usec;

                struct SlotInfoASlot {
                    unsigned int period;
                    unsigned int length;
                    unsigned int shift;
                    unsigned int shift_min;
                    unsigned int shift_width;
                } slot_aslot;

                void init(const NodeID & node_id,
                          const el::ml::LocalChanID & lc_id,
                          const el::ml::NetworkID & network_id,
                          const Subject & subj,
                          period_t period_us,
                          mel_t max_event_length_bytes,
                          repetition_t repetition,
                          uint64_t * slot_bound_start_us,
                          uint64_t * slot_bound_end_us,
                          uint64_t cycle_start_time_us,
                          const NetworkTimingConfig & net) {
                    this->node_id = node_id;
                    this->lc_id = lc_id;
                    this->network_id = network_id;
                    this->subject = subj;

                    FAMOUSO_ASSERT(period_us % net.plan_granul_us == 0);
                    this->slot_usec.period = period_us;
                    this->slot_aslot.period = period_us / net.plan_granul_us;

                    net.calc_slot_durations(max_event_length_bytes, repetition, this->slot_usec.length, this->slot_usec.tx_window);
                    this->slot_aslot.length = div_round_up(this->slot_usec.length, net.plan_granul_us);

                    if (slot_bound_start_us && slot_bound_end_us) {
                        slot_bounds_given = true;
                        uint64_t length_us = increase_to_multiple(slot_usec.length, net.plan_granul_us);
                        this->slot_usec.shift_min = increase_to_multiple(*slot_bound_start_us, net.plan_granul_us);
                        this->slot_usec.shift_max = reduce_to_multiple(*slot_bound_end_us, net.plan_granul_us);
                        if (slot_usec.shift_min + length_us <= slot_usec.shift_max) {

                            uint64_t a = increase_by_multiple_above(slot_usec.shift_min, (uint64_t)period_us, cycle_start_time_us) - cycle_start_time_us;
                            this->slot_aslot.shift_min   = div_round_up(a,
                                                                        net.plan_granul_us);
                            this->slot_aslot.shift_min  %= slot_aslot.period;
                            this->slot_aslot.shift_width = div_round_up(slot_usec.shift_max - length_us - slot_usec.shift_min,
                                                                    net.plan_granul_us);
                            FAMOUSO_ASSERT(0 <= slot_aslot.shift_min && slot_aslot.shift_min < slot_aslot.period);
                        } else {
                            // TODO: Handle this properly
                            log();
                            printf("Slot phase selection range to small!!\n");
                            exit(1);
                        }
                    } else {
                        slot_bounds_given = false;
                    }

                    // Calculated during reservation
                    this->slot_usec.shift = 0;
                    this->slot_aslot.shift = 0;

                    this->status = waiting_for_reservation;
                }

                void init_reserv(unsigned int shift_aslots,
                                 const NetworkTimingConfig & net_param) {
                    FAMOUSO_ASSERT(status == waiting_for_reservation);

                    this->slot_aslot.shift = shift_aslots;
                    this->slot_usec.shift = shift_aslots * net_param.plan_granul_us;
                }

                void log() {
                    using namespace ::logging;
#if 1
                    log::emit()
                        << "- RTCC: "
                        << "Subject " << subject
                        << ", LocalChanID " << log::hex << lc_id << ", NodeID " << node_id << "\n"
                        << "\tSlot Period: " << log::dec << slot_usec.period << " us -> "  << slot_aslot.period << " aSlots\n"
                        << "\tSlot Length: " << slot_usec.length << " us -> "  << slot_aslot.length << " aSlots\n";
                    if (slot_bounds_given) {
                        log::emit()
                            << "\tSlot phase selection range: [" << slot_usec.shift_min << " us, " << slot_usec.shift_max << " us] -> ["
                            << slot_aslot.shift_min << " aSlots, " << slot_aslot.shift_min + slot_aslot.shift_width - 1 << " aSlots]\n";
                    }
                    if (status == waiting_for_reservation) {
                        log::emit() << "\t==> Reservation failed!!!\n";
                    } else {
                        log::emit()
                            << "\tSlot Phasing: " << slot_aslot.shift << " aSlots ->  " << slot_usec.shift << " us\n"
                            << "\tSlot TX Start Window: " << slot_usec.tx_window << " us\n";
                        if (status == waiting_for_subscriber) {
                            log::emit() << "\t==> Reserved\n";
                        } else if (status == delivering) {
                            log::emit() << "\t==> Reserved and in use\n";
                        } else if (status == waiting_for_poll_master_ack) {
                            log::emit() << "\t==> Reserved, starting delivery\n";
                        } else if (status == waiting_for_producer_ack) {
                            log::emit() << "\t==> Reserved, stopping delivery\n";
                        }
                    }
                }
#else
                    // Short output
                    log::emit()
                        << "- RTCC: "
                        << "Subject " << subject
                        << ", NodeID " << node_id << "\n"
                        << "\tPeriod = " << log::dec << slot_aslot.period << " aSlots,\tLength = " << slot_aslot.length << " aSlots\n";
                    if (status == waiting_for_reservation) {
                        log::emit() << "\t==> Reservation failed!!!\n";
                    } else {
                        log::emit() << "\t==> Reserved\t\t"
                            << "Phasing = " << slot_aslot.shift << " aSlots\n";
                    }
                }
#endif
            };

        } // namespace rt_net_sched
    } // namespace mw
} // namespace famouso

#endif // __REALTIMETXCHANNEL_H_3A33121E118371__

