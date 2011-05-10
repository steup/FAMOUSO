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

#ifndef __CHANNELREQUIREMENTSEVENT_H_D0840388D680E6__
#define __CHANNELREQUIREMENTSEVENT_H_D0840388D680E6__

#include "mw/el/ml/event_types.h"
#include "mw/el/ml/SerializationBase.h"
#include "mw/common/Subject.h"
#include "mw/common/NodeID.h"
#include "mw/el/ml/NetworkID.h"
#include "mw/el/ml/LocalChanID.h"
#include "mw/el/ml/ChannelReservationData.h"

namespace famouso {
    namespace mw {
        namespace el {
            namespace ml {

                // ChannelSet
                class ChannelRequirementsEvent : public SerializationBase {
                            // Once: Message-Type, NodeID, count of NetworkIDs, NetworkIDs
                            // Per channel: Subject, This (PubID, 4B), Requirements
                    public:

                        static uint16_t header_size() {
                            return 1 + sizeof(NodeID) + 1 + 1 * sizeof(NetworkID) /* TODO: NetworkIDs */;
                        }

                        static uint16_t entry_size(uint16_t attribute_size) {
                            return sizeof(Subject) + sizeof(uint64_t) + attribute_size;
                        }
                };

                class ChannelRequirementsWriter : public ChannelRequirementsEvent {
                    public:
                        ChannelRequirementsWriter() {
                            length_add(header_size());
                        }

                        void write_head(uint8_t message_type, const NodeID & nodeID, const NetworkID & network_id) {
                            FAMOUSO_ASSERT(further_space(header_size()));
                            write_u8(message_type);
                            write_uid(nodeID);

                            // Count of NetworkIDs
                            write_u8(1);
                            // TODO: support multi-net
                            write_uid(network_id);
                        }

                        template <class EventChannel>
                        void write_channel(const EventChannel & ec) {
                            //FAMOUSO_ASSERT(further_space(entry_size(as)));
                            // Subject
                            write_uid(ec.subject());
                            // Local channel ID: this pointer
                            uint64_t ec_this = reinterpret_cast<uint64_t>(&ec);
                            write_uid(UID(reinterpret_cast<uint8_t *>(&ec_this)));
                            // Attributes
                            uint8_t *& dp = data_pointer();
                            dp += ec.get_requirements(dp);
                        }

                        template <class AttribSet>
                        void write_channel(const Subject & subj, const LocalChanID & lc_id) {
                            //FAMOUSO_ASSERT(further_space(entry_size(as)));
                            write_uid(subj);
                            // Local channel ID: this pointer
                            write_uid(lc_id);
                            // Attributes
                            uint8_t *& dp = data_pointer();
                            new (dp) AttribSet;
                            dp += sizeof(AttribSet);
                        }
                };

                class ChannelRequirementsReader : public ChannelRequirementsEvent {
                    public:
                        ChannelRequirementsReader(uint8_t * buffer, uint16_t length) {
                            init(buffer, length);
                        }

                        void read_head(uint8_t & message_type, NodeID & nodeID, NetworkID & network_id) {
                            FAMOUSO_ASSERT(further_space(header_size()));
                            read_u8(message_type);
                            read_uid(nodeID);

                            // Count of NetworkIDs
                            uint8_t nid_count;
                            read_u8(nid_count);
                            FAMOUSO_ASSERT(nid_count == 1);
                            // TODO: support multi-net
                            read_uid(network_id);
                        }

                        bool further_channel() {
                            return further_space(entry_size(1));
                        }

                        // as points into event data buffer
                        void read_channel(Subject & subj, LocalChanID & lc_id, const attributes::AttributeSet<> *& as) {
                            FAMOUSO_ASSERT(further_space(entry_size(1)));   // does not catch all buffer overflow leaks
                            read_uid(subj);
                            read_uid(lc_id);
                            read_as(as);
                        }
                };

            } // namespace ml
        } // namespace el
    } // namespace mw
} // namespace famouso

#endif // __CHANNELREQUIREMENTSEVENT_H_D0840388D680E6__

