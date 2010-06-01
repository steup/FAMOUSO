/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Marcus Foerster <MarcusFoerster1@gmx.de>
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

#ifndef _Find_h_
#define _Find_h_

#include <stdint.h>

#include "util/endianness.h"
#include "mw/attributes/detail/AttributeElementHeader.h"
#include "mw/attributes/detail/SystemIDs.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                template <typename Attr>
                static inline const Attr* find(const uint8_t* data) {
                    return (find<Attr>(const_cast<uint8_t*>(data)));
                }

                template <typename Attr>
                Attr* find(uint8_t* data) {
                    // The number of attributes contained in the given sequence
                    uint16_t seqLen;

                    // Determine sequence length
                    if ((data[0] & 0x80) == 0) {
                        // Sequence header is unextended
                        seqLen = (data[0] & 0x7F);

                        ++data;
                    } else {
                        // Sequence header is extended

                        seqLen = *(reinterpret_cast<uint16_t*>(data));
                        seqLen = ntohs(seqLen) & 0x7FFF;

                        data += 2;
                    }

                    // Current attribute index starting at 0
                    uint16_t index = 0;

                    // The length of the currently decoded attribute
                    uint16_t length;

                    AttributeElementHeader* header;

                    while (index < seqLen) {
                        // Interpret the current pointer as an attribute header
                        header = reinterpret_cast<AttributeElementHeader*>(data);

                        if (header->category == SystemIDs::nonSystem) {
                            // Non-system attribute

                            // In this case we have to proceed to the type field to check it
                            //  against the intended attribute's ID

                            // This means that we have to read the length first
                            if (header->extension) {
                                length = *(reinterpret_cast<uint16_t*>(data++));
                                length = ntohs(length) & 0x07FF;
                            } else {
                                length = header->length;
                            }

                            ++data;

                            // Now we're pointing at the type
                            if (*(data++) == Attr::id) {
                                // If the types matches the intended attribute, we interpret
                                //  the current data chunk as the attribute and return it
                                return (reinterpret_cast<Attr*>(header));
                            }
                        } else {
                            // System attributes

                            // For system attributes, the ID is always written at the
                            //  beginning of the header, so we can check this right now
                            if (header->category == Attr::id) {
                                return (reinterpret_cast<Attr*>(header));
                            }

                            if (header->valueOrLengthSwitch) {
                                // If the VOL flag is set, the value (respective a part of it)
                                //  is contained in the header

                                // If the value is extended, we have to skip another byte
                                if (header->extension) ++data;

                                // The length (which is used to skip below) is 1 in this case
                                //  (either the header byte itself or the extended byte is
                                //  considered as the value)
                                length = 1;
                            } else {
                                // The VOL flag isn't set, so the length is contained in the header

                                if (header->extension) {
                                    length = *(reinterpret_cast<uint16_t*>(data++));
                                    length = ntohs(length) & 0x03FF;
                                } else {
                                    length = header->valueOrLength;
                                }

                                ++data;
                            }
                        }

                        // Skip the data bytes of the current attribute
                        data += length;

                        // Increment the index since we handle the next attribute in the sequence
                        ++index;
                    }

                    // If we iterated the complete attribute sequence the intended attribute
                    //  could not be found and NULL is returned
                    return (reinterpret_cast<Attr*>(NULL));
                }

            } // end namespace detail
        } // end namespace attributes
    } // end namespace attributes
} // end namespace attributes

#endif
