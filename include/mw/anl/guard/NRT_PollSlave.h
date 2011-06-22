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

#ifndef __NRT_POLLSLAVE_H_69BEA0DB97942E__
#define __NRT_POLLSLAVE_H_69BEA0DB97942E__

#include "mw/common/Subject.h"
#include "timefw/Time.h"
#include "timefw/Dispatcher.h"

namespace famouso {
    namespace mw {
        namespace guard {

            /*!
             *  \brief  NRT policy for network guard: slave for polled NRT
             *          packets
             *
             *  \todo   Implement NRT_PollSlave and NRT_PollMaster.
             */
            template <class NL>
            class NRT_PollSlave : public NL {

                    volatile bool nrt_ar_expired;
                    volatile uint64_t nrt_ar_expire_time;

                    /// Grants NRT access for a given duration
                    void grant_nrt_access_right(timefw::Time duration) {
                        nrt_ar_expire_time = (timefw::TimeSource::current() + duration).get_usec();
                        nrt_ar_expired = false;
                        ::logging::log::emit() << "[NRT-Poll-Slave] granting access right at " << timefw::TimeSource::current() << " for " << duration << "\n";
                    }

                protected:
                    /// Returns when the NRT packet can be transmitted
                    void ensure_nrt_access_right() {
                        ::logging::log::emit() << "[NRT-Poll-Slave] waiting for nrt access right...\n";
                        while (1) {
                            if (!nrt_ar_expired) {
                                // We may have access
                                if (nrt_ar_expire_time > timefw::TimeSource::current().get_usec()) {
                                    // Access granted
                                    ::logging::log::emit() << "[NRT-Poll-Slave] ...access granted\n";
                                    return;
                                } else {
                                    nrt_ar_expired = true;
                                }
                            }
                            if (!timefw::Dispatcher::instance().yield_for_rt())
                                return;     // Interrupted by signal
                        }
                    }

                    /// Short network name of the poll channel
                    typename NL::SNN poll_snn;

                    /// Process poll packet (returns whether sent to this node)
                    bool process_poll_packet(const typename NL::Packet_t & packet) {
                        if (packet.data_length == sizeof(NodeID) + sizeof(uint32_t)) {
                            NodeID * node_id = reinterpret_cast<NodeID *>(packet.data);
                            if (*node_id == getNodeID<void>()) {
                                uint32_t grant_us = ntohl(*reinterpret_cast<uint32_t *>(packet.data + sizeof(NodeID)));
                                grant_nrt_access_right(timefw::Time::usec(grant_us));
                                return true;
                            }
                        }
                        return false;
                    }

                public:
                    /// Initialization of this and lower layers
                    void init() {
                        nrt_ar_expired = true;
                        NL::init();
                        NL::bind(Subject("NRTPoll!"), poll_snn);
                    }

                    /*!
                     *  \brief  Check if NL received a poll event and process it
                     *  \return Whether there was a poll event.
                     */
                    bool handle_poll_event() {
                        if (NL::lastPacketSNN() == poll_snn) {
                            typename NL::Packet_t packet;
                            NL::take(packet);
                            process_poll_packet(packet);
                            return true;
                        } else {
                            return false;
                        }
                    }

                    /// Policy invoked by AbstactNetworkLayer on event process request
                    struct EventProcessRequestPolicy {
                        static bool process(NRT_PollSlave & network_guard) {
                            // The incoming event may be a poll event. In that case
                            // the event is processed here. That is why further
                            // event fetching is not needed which is signalled by
                            // returning false.
                            return !network_guard.handle_poll_event();
                        }
                    };

                    // TODO: add NRT_PollMaster policy and move it to that policy
                    void deliver(const typename NL::Packet_t& p) {
                        // Only deliver poll messages to net, that are not addressed to this node.
                        if (p.snn == poll_snn) {
                            if (!process_poll_packet(p))
                                NL::deliver(p);
                        }
                    }

            };


        } // namespace guard
    } // namespace mw
} //namespace famouso

#endif // __NRT_POLLSLAVE_H_69BEA0DB97942E__

