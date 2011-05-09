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

#ifndef __NETWORKTIMINGCONFIG_H_19C5E85FCBCF9F__
#define __NETWORKTIMINGCONFIG_H_19C5E85FCBCF9F__

#include <stdint.h>

#include "value_types.h"
#include "math.h"


namespace famouso {
    namespace mw {
        namespace rt_net_sched {

            struct NetworkTimingConfig {

                uint64_t clock_granul_ns;

                /// nanosec per atomic slot
                uint64_t plan_granul_ns;

                /// microsec per atomic slot
                uint64_t plan_granul_us;

                uint16_t mtu;

                bool need_free_slots;

                // Parameters in nano seconds (to support short times)
                uint64_t trigger_to_usf_ns;
                uint64_t usf_to_txready_ns;
                uint64_t med_contention_ns;
                uint64_t tx_overhead_ns;
                uint64_t tx_per_byte_ns;
                uint64_t tx_min_ns;
                // Bei berechnung aufrunden



                void calc_slot_durations(mel_t event_length,
                                         repetition_t repetition,
                                         uint64_t & slot_length_us,
                                         uint64_t & slot_tx_window_us) const {
                    uint64_t non_msg_bytes_ns = trigger_to_usf_ns + usf_to_txready_ns + med_contention_ns + tx_overhead_ns;
                    unsigned int fragments;
                    unsigned int last_fragment_length;
                    if (event_length == 0) {
                        fragments = 1;
                        last_fragment_length = 0;
                    } else {
                        fragments = div_round_up(event_length, mtu - 1);
                        last_fragment_length = event_length - (fragments - 1) * (mtu - 1) + 1;
                    }

                    uint64_t max_msg_ns = non_msg_bytes_ns + mtu * tx_per_byte_ns;
                    if (max_msg_ns < tx_min_ns) {
                        max_msg_ns = tx_min_ns;
                    }

                    uint64_t last_msg_ns = non_msg_bytes_ns + last_fragment_length * tx_per_byte_ns;
                    if (last_msg_ns < tx_min_ns) {
                        last_msg_ns = tx_min_ns;
                    }

                    // Pro frame aufrundung auf Uhrengranul weggelassen -> mit Micha diskutieren, wie in DA Text zu schreiben
                    uint64_t tx_window_ns = increase_to_multiple((repetition + 1) * (fragments - 1) * max_msg_ns +
                                                                            repetition * last_msg_ns +
                                                                            trigger_to_usf_ns,
                                                                      clock_granul_ns);
                    uint64_t slot_len_ns = tx_window_ns + usf_to_txready_ns + med_contention_ns + tx_overhead_ns + last_fragment_length * tx_per_byte_ns +
                                                    clock_granul_ns /* acuuracy reserve */;

                    slot_length_us = div_round_up(slot_len_ns, 1000);
                    slot_tx_window_us = div_round_up(tx_window_ns, 1000);
                }
            };

            static inline NetworkTimingConfig CanNetworkTimingConfig(
                        uint64_t bits_per_second,
                        uint64_t clock_granul_microsec,
                        uint64_t plan_granul_microsec,
                        uint64_t max_trigger_to_usf_time_nanosec,
                        uint64_t max_usf_to_txready_time_nanosec) {
                NetworkTimingConfig ntc;
                ntc.clock_granul_ns = clock_granul_microsec * 1000;
                ntc.plan_granul_us = plan_granul_microsec;
                ntc.plan_granul_ns = plan_granul_microsec * 1000;
                ntc.mtu = 8;
                ntc.need_free_slots = false;

                ntc.trigger_to_usf_ns = max_trigger_to_usf_time_nanosec;
                ntc.usf_to_txready_ns = max_usf_to_txready_time_nanosec;
                uint64_t ns_per_bit = div_round_up(1000000000lu, bits_per_second);
                ntc.tx_overhead_ns = 111 * ns_per_bit;
                ntc.tx_per_byte_ns = 10 * ns_per_bit;
                ntc.tx_min_ns = 0;
                ntc.med_contention_ns = ntc.tx_overhead_ns + ntc.mtu * ntc.tx_per_byte_ns;

                return ntc;
            }

            static inline NetworkTimingConfig EthernetNetworkTimingConfig(
                        uint64_t bits_per_second,
                        uint64_t clock_granul_microsec,
                        uint64_t plan_granul_microsec,
                        uint64_t max_trigger_to_usf_time_nanosec,
                        uint64_t max_usf_to_txready_time_nanosec) {
                NetworkTimingConfig ntc;
                ntc.clock_granul_ns = clock_granul_microsec * 1000;
                ntc.plan_granul_us = plan_granul_microsec;
                ntc.plan_granul_ns = plan_granul_microsec * 1000;
                ntc.mtu = 1500 - 9;     // 9 Bytes used by network layer for header info
                // TODO <- header
                ntc.need_free_slots = true;

                ntc.trigger_to_usf_ns = max_trigger_to_usf_time_nanosec;
                ntc.usf_to_txready_ns = max_usf_to_txready_time_nanosec;
                uint64_t ns_per_bit = div_round_up(1000000000lu, bits_per_second);
                ntc.tx_overhead_ns = 26 * 8 * ns_per_bit;
                ntc.tx_per_byte_ns = 8 * ns_per_bit;
                if (bits_per_second == 1000000000)
                    ntc.tx_min_ns = (512 + 8) * 8 * ns_per_bit;
                else
                    ntc.tx_min_ns = (64 + 8) * 8 * ns_per_bit;
                ntc.med_contention_ns = 0;

                return ntc;
            }

        } // namespace rt_net_sched
    } // namespace mw
} // namespace famouso

#endif // __NETWORKTIMINGCONFIG_H_19C5E85FCBCF9F__

