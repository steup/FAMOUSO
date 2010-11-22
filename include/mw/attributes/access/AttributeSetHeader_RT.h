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

#ifndef _Attribute_Set_Header_RT_h_
#define _Attribute_Set_Header_RT_h_

#include <stdint.h>

#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/integral_c.hpp"

#include "mw/attributes/detail/AttributeCompileErrors.h"

#include "util/endianness.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace access {

                struct AttributeSetHeader_RT {
                    private:
                        AttributeSetHeader_RT() {
                            // Visibility
                        }

                    public:
                        const bool isExtended() const {
                            return ((*reinterpret_cast<const uint8_t* const>(this) & 0x80) != 0);
                        }

                        const uint16_t get() const {
                        	const uint8_t* const data = reinterpret_cast<const uint8_t* const>(this);

                            if (isExtended()) {
                                // Get the first two bytes, convert them to host byte order and mask the
                                //  extension bit
                                return (ntohs(*(reinterpret_cast<const uint16_t* const>(&data[0]))) & 0x7FFF);
                            } else {
                                // Simply return the first (and only) byte, the check above assures that
                                //  only the lower seven bits of this byte are set
                                return (data[0]);
                            }
                        }

                        bool set(uint16_t newSize) {
                            // Check if the extension bit is set in the current representation
                            const bool currentExtension = isExtended();

                            // Check if the extension bit must be set for the new size
                            const bool targetExtension = (newSize > 0x7F);

                            // If the old size was not extended and the new one has to be, we cannot
                            //  perform the operation since we need more bytes than available
                            if (!currentExtension && targetExtension) {
                                return (false);
                            }

                            writeSize(newSize, targetExtension);

                            return (true);
                        }

                    private:
                        void writeSize(const uint16_t size, const bool extension) {
                            uint8_t* const data = reinterpret_cast<uint8_t* const>(this);

                            // Depending on whether the sequence header is extended either 1
                            //  or 2 two bytes must be written accordingly
                            if (extension) {
                                // Convert the length to network byte order and set the
                                //  extension bit
                                const uint16_t tmpSize = htons(size | 0x8000);
                                // Assign the converted value to the array
                                *(reinterpret_cast<uint16_t*>(data)) = tmpSize;
                            } else {
                                // Write the lower 7 bits of the sequence size into the first
                                //  and only byte (The extension flag assures that the given
                                //  sequence size fits 7 bits)
                                data[0] = (size & 0x7F);
                            }
                        }
                };

            } // end namespace access
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _Attribute_Set_Header_
