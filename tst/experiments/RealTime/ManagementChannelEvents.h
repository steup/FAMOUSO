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


#ifndef __MANAGEMENTCHANNELEVENTS_H_7DEF77D0BD5BA1__
#define __MANAGEMENTCHANNELEVENTS_H_7DEF77D0BD5BA1__

#include <stdint.h>
#include "mw/attributes/AttributeSet.h"
#include "mw/common/UID.h"
#include "mw/common/Subject.h"
#include "mw/el/ml/ChannelReservationData.h"

// TODO: split file: in detail oder events Reservation, ChannelRequirements, SeralizationBase


namespace famouso {
namespace mw {
namespace el {
namespace manchan {
// detail?

enum { announce_msg = 0, subscribe_msg = 1, rt_reservation_msg = 2 };

// TODO: bei readern buffer/length-checks auch bei abgeschalteten asserts (security)
class SerializationBase {
        uint16_t length;
        union {
            uint8_t * u8;
            uint16_t * u16;
            uint32_t * u32;
            uint64_t * u64;
        } data;
        uint8_t * data_end;

    public:
        uint16_t get_length() const {
            return length;
        }

        // Length accumulation
        void length_add(uint16_t bytes) {
            length += bytes;
        }

        // Assign buffer of current length. See \ref get_length().
        void init(uint8_t * buffer) {
            data.u8 = buffer;
            data_end = buffer + length;
        }

        void init(uint8_t * buffer, uint16_t len) {
            length = len;
            init(buffer);
        }

        bool initialized() {
            return data.u8 != 0;
        }

        uint8_t * get_rewind_mark() {
            return data.u8;
        }

        void rewind(uint8_t * rewind_mark) {
            data.u8 = rewind_mark;
        }

    protected:
        SerializationBase() {
            length = 0;
            data.u8 = 0;
            data_end = 0;
        }

        bool further_space(uint16_t bytes) const {
            return data_end - data.u8 >= bytes;
        }
        bool buffer_full() const {
            return data_end  == data.u8;
        }
        uint8_t *& data_pointer() {
            return data.u8;
        }

        template <typename T>
        void write(const T & val) {
            new (data.u8) T(val);
            data.u8 += sizeof(T);
        }
        /*
        void write_as(const attributes::AttributeSet<> & as) {
            uint16_t len = as.getSize();
            memcpy(data.u8, &as, len);
            data.u8 += len;
        }
        */
        void write_u8(uint8_t val) {
            *data.u8 = val;
            ++data.u8;
        }
        void write_u16(uint16_t val) {
            *data.u16 = htons(val);
            ++data.u16;
        }
        void write_u32(uint32_t val) {
            *data.u32 = htonl(val);
            ++data.u32;
        }
        void write_u64(uint64_t val) {
            *data.u64 = htonll(val);
            ++data.u64;
        }

        template <typename T>
        void read(T & val) {
            val = *reinterpret_cast<T *>(data.u8);
            data.u8 += sizeof(T);
        }
        void read_as(const attributes::AttributeSet<> *& as) {
            as = reinterpret_cast<attributes::AttributeSet<> *>(data.u8);
            data.u8 += as->length();
        }
        void read_u8(uint8_t & val) {
            val = *data.u8;
            ++data.u8;
        }
        void read_u16(uint16_t & val) {
            val = ntohs(*data.u16);
            ++data.u16;
        }
        void read_u32(uint32_t & val) {
            val = ntohl(*data.u32);
            ++data.u32;
        }
        void read_u64(uint64_t & val) {
            val = ntohll(*data.u64);
            ++data.u64;
        }
};


// ChannelSet
class ChannelRequirements : public SerializationBase {
            // Once: Message-Type, NodeID, count of NetworkIDs, NetworkIDs
            // Per channel: Subject, This (PubID, 4B), Requirements
    public:

        static uint16_t header_size() {
            return 1 + sizeof(UID) + 1 + 0 /* TODO: NetworkIDs */;
        }

        static uint16_t entry_size(uint16_t attribute_size) {
            return sizeof(Subject) + sizeof(uint64_t) + attribute_size;
        }
};

class ChannelRequirementsWriter : public ChannelRequirements {
    public:
        ChannelRequirementsWriter() {
            length_add(header_size());
        }

        void write_head(uint8_t message_type, const UID & nodeID) {
            FAMOUSO_ASSERT(further_space(header_size()));
            write_u8(message_type);
            write(nodeID);

            // Count of NetworkIDs
            write_u8(0);
            // TODO: networkIDs
        }

        template <class EventChannel>
        void write_channel(const EventChannel & ec) {
            //FAMOUSO_ASSERT(further_space(entry_size(as)));
            // Subject
            write(ec.subject());
            // Local channel ID: this pointer
            write_u64(reinterpret_cast<uint64_t>(&ec));
            // Attributes
            uint8_t *& dp = data_pointer();
            dp += ec.get_requirements(dp);
        }
};

class ChannelRequirementsReader : public ChannelRequirements {
    public:
        ChannelRequirementsReader(uint8_t * buffer, uint16_t length) {
            init(buffer, length);
        }

        void read_head(uint8_t & message_type, UID & nodeID) {
            FAMOUSO_ASSERT(further_space(header_size()));
            read_u8(message_type);
            read(nodeID);

            // Count of NetworkIDs
            uint8_t nid_count;
            read_u8(nid_count);
            // TODO: networkIDs
        }

        bool further_channel() {
            return further_space(entry_size(1));
        }

        // as points into event data buffer
        void read_channel(Subject & subj, uint64_t & lc_id, const attributes::AttributeSet<> *& as) {
            FAMOUSO_ASSERT(further_space(entry_size(1)));   // does not catch all buffer overflow leaks
            read(subj);
            read_u64(lc_id);
            read_as(as);
        }
};

class Reservation : public SerializationBase {
    public:
        // Once: Message-Type, NodeID, TODO: NetworkID
        // Per channel: This, StartTime, SlotTimespan (letzter erlaubter Sendezeitpunkt - erster erlaubter)

        static uint16_t header_size() {
            return 1 + sizeof(UID);
        }

        static uint16_t entry_size() {
            return sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint32_t);
        }

        static uint16_t size(uint16_t channel_count) {
            return header_size() + channel_count * entry_size();
        }
};

class ReservationWriter : public Reservation {
    public:
        ReservationWriter(uint8_t * buffer, uint16_t length) {
            init(buffer, length);
        }

        ~ReservationWriter() {
            FAMOUSO_ASSERT(buffer_full());
        }

        void write_head(const UID & nodeID) {
            FAMOUSO_ASSERT(further_space(header_size()));
            write_u8(rt_reservation_msg);
            write(nodeID);
        }

        void write_channel(const ml::ChannelReservationData & crd) {
            FAMOUSO_ASSERT(further_space(entry_size()));
            write_u64(crd.lc_id);
            write_u64(crd.tx_ready_time);
            write_u32(crd.tx_window_time);
        }
};

class ReservationReader : public Reservation {
    public:
        ReservationReader(uint8_t * buffer, uint16_t length) {
            init(buffer, length);
        }

        void read_head(UID & nodeID) {
            FAMOUSO_ASSERT(further_space(header_size()));
            uint8_t msg_type;
            read_u8(msg_type);
            FAMOUSO_ASSERT(msg_type == rt_reservation_msg);
            read(nodeID);
        }

        bool further_channel() {
            return further_space(entry_size());
        }

        void read_channel(ml::ChannelReservationData & crd) {
            FAMOUSO_ASSERT(further_space(entry_size()));
            // This-Pointer
            read_u64(crd.lc_id);
            // Start-Time
            read_u64(crd.tx_ready_time);
            // Slot-Length-Time
            read_u32(crd.tx_window_time);
        }
};


}
}
}
}

#endif // __MANAGEMENTCHANNELEVENTS_H_7DEF77D0BD5BA1__

