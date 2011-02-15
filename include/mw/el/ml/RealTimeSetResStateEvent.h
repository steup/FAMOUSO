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

#ifndef __REALTIMESETRESSTATEEVENT_H_12144676BFACB6__
#define __REALTIMESETRESSTATEEVENT_H_12144676BFACB6__

#include "mw/el/ml/event_types.h"
#include "mw/el/ml/SerializationBase.h"
#include "mw/common/Subject.h"
#include "mw/common/NodeID.h"
#include "mw/el/ml/NetworkID.h"
#include "mw/el/ml/LocalChanID.h"

namespace famouso {
    namespace mw {
        namespace el {
            namespace ml {

                /*!
                 *  \brief  Real time reservation event writer and reader
                 *
                 *  Use it either for reading or for writing.
                 *  Always first read/write the head, afterwards read/write
                 *  the tail. To read again, you have to reinitialize the
                 *  object.
                 */
                class RealTimeSetResStateEvent : public SerializationBase {

                        // TODO: bei read buffer/length-checks auch bei abgeschalteten asserts (security)
                    public:

                        static uint16_t header_size() {
                            return 1 + sizeof(NodeID);
                        }

                        static uint16_t tail_size(bool reserv) {
                            if (reserv)
                                return sizeof(LocalChanID) + sizeof(NetworkID) + 8 + 4;
                            else
                                return sizeof(LocalChanID) + sizeof(NetworkID);
                        }

                        static uint16_t size(bool reserv) {
                            return header_size() + tail_size(reserv);
                        }

                        RealTimeSetResStateEvent(uint8_t * buffer, uint16_t length) {
                            init(buffer, length);
                        }

                        void write_head(uint8_t type, const NodeID & nodeID) {
                            FAMOUSO_ASSERT(further_space(header_size()));
                            write_u8(type);
                            write_uid(nodeID);
                        }

                        uint8_t read_head(NodeID & nodeID) {
                            FAMOUSO_ASSERT(further_space(header_size()));
                            uint8_t type;
                            read_u8(type);
                            read_uid(nodeID);
                            if (type == rt_reservation_event || type == rt_no_reservation_event || type == rt_no_subscriber_event)
                                return type;
                            else
                                return unsupported_event;
                        }

                        void write_reserv_tail(const LocalChanID & lc_id, const NetworkID & network_id,
                                               uint64_t tx_ready_time, uint32_t tx_window_time) {
                            FAMOUSO_ASSERT(further_space(tail_size(true)));
                            write_uid(lc_id);
                            write_uid(network_id);
                            write_u64(tx_ready_time);
                            write_u32(tx_window_time);
                        }

                        void read_reserv_tail(ChannelReservationData & crd) {
                            FAMOUSO_ASSERT(further_space(tail_size(true)));
                            read_uid(crd.lc_id);
                            read_uid(crd.network_id);
                            read_u64(crd.tx_ready_time);
                            read_u32(crd.tx_window_time);
                        }

                        void write_no_reserv_tail(const LocalChanID & lc_id, const NetworkID & network_id) {
                            FAMOUSO_ASSERT(further_space(tail_size(false)));
                            write_uid(network_id);
                            write_uid(lc_id);
                        }

                        void read_no_reserv_tail(ChannelReservationData & crd) {
                            FAMOUSO_ASSERT(further_space(tail_size(false)));
                            read_uid(crd.network_id);
                            read_uid(crd.lc_id);
                        }
                };

            } // namespace ml
        } // namespace el
    } // namespace mw
} // namespace famouso

#endif // __REALTIMESETRESSTATEEVENT_H_12144676BFACB6__

