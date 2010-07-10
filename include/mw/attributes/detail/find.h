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

#include "mw/attributes/detail/AttributeSetHeader.h"
#include "mw/attributes/detail/AttributeHeader.h"

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
                    // The number of bytes needed by the attributes
                    //  contained in the given sequence
                    uint16_t seqLen;

                    {
                        // TODO: Think about a special API accessor type instead of using
                        //  the template type with zero manually
                        const AttributeSetHeader<0>* const setHeader =
                                reinterpret_cast<const AttributeSetHeader<0>* const>(&data[0]);

                        seqLen = setHeader->get();

                        data += (setHeader->isExtended() ? 2 : 1);
                    }

                    // The pointer were the given sequence ends
                    const uint8_t* const targetPtr = data + seqLen;

                    // TODO: The same about the API accessor type here, using Attr for
                    //  the type argument is not necessary it could be any arbitrary
                    //  attribute type
                    AttributeHeader<Attr>* header;

                    while (data < targetPtr) {
                        header = reinterpret_cast<AttributeHeader<Attr>*>(data);

                        // Check if the encoded attribute fits the given one
                        if ((header->isSystem() == Attr::isSystem) && (header->getID() == Attr::id)) {
                            return (reinterpret_cast<Attr*>(header));
                        }

                        // We let the attribute class determine its overall size to skip it
                        data += reinterpret_cast<const Attr* const>(header)->size();
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
