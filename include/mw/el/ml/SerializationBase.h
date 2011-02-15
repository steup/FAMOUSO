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

#ifndef __SERIALIZATIONBASE_H_4BAE515BA1A553__
#define __SERIALIZATIONBASE_H_4BAE515BA1A553__

#include <stdint.h>
#include "mw/common/UID.h"
#include "mw/attributes/AttributeSet.h"

namespace famouso {
    namespace mw {
        namespace el {
            namespace ml {

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

                        void write_uid(const UID & val) {
                            new (data.u8) UID(val);
                            data.u8 += sizeof(UID);
                        }
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

                        void read_uid(UID & val) {
                            val = *reinterpret_cast<UID *>(data.u8);
                            data.u8 += sizeof(UID);
                        }
                        void read_as(const attributes::AttributeSet<> *& as) {
                            as = reinterpret_cast<const attributes::AttributeSet<> *>(data.u8);
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

            } // namespace ml
        } // namespace el
    } // namespace mw
} // namespace famouso

#endif // __SERIALIZATIONBASE_H_4BAE515BA1A553__

