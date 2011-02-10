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
#include "../math.h"


namespace famouso {
    namespace mw {
        namespace rt_net_sched {

            struct RealTimeTxChannel {
                enum Status {
                    waiting_for_subscriber,
                    reserved,
                    waiting_for_ressources,
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

                struct SlotInfoUSec {
                    uint64_t period;
                    uint64_t length;
                    uint64_t tx_window;       // relative to shift
                    uint64_t shift;
                } slot_usec;

                struct SlotInfoASlot {
                    unsigned int period;
                    unsigned int length;
                    unsigned int shift;
                } slot_aslot;

                void init(const NodeID & node_id,
                          const el::ml::LocalChanID & lc_id,
                          const el::ml::NetworkID & network_id,
                          const Subject & subj,
                          period_t period_us,
                          mel_t max_event_length_bytes,
                          repetition_t repetition,
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

                    // Calculated during reservation
                    this->slot_usec.shift = 0;
                    this->slot_aslot.shift = 0;

                    this->status = waiting_for_subscriber;
                }

                void init_reserv(unsigned int shift_aslots,
                                 const NetworkTimingConfig & net_param) {
                    FAMOUSO_ASSERT(status == waiting_for_subscriber ||
                                   status == waiting_for_ressources);

                    this->slot_aslot.shift = shift_aslots;
                    this->slot_usec.shift = shift_aslots * net_param.plan_granul_us;
                }

                void log() {
                    using namespace ::logging;
                    log::emit()
                        << "- RT Publisher " << log::hex << lc_id << ": subject " << subject
                        << ", node_id " << node_id
                        << ";\tSlot period " << log::dec << slot_aslot.period << " (" << slot_usec.period << " us), "
                        << "length " << slot_aslot.length << " (" << slot_usec.length << " us), "
                        << "shift " << slot_aslot.shift << " (" << slot_usec.shift << " us)"
                        << ::logging::log::endl;
                }
            };

        } // namespace rt_net_sched
    } // namespace mw
} // namespace famouso

#endif // __REALTIMETXCHANNEL_H_3A33121E118371__

