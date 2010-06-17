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

#ifndef _Attribute_Set_Header_
#define _Attribute_Set_Header_

#include <stdint.h>

#include "boost/mpl/eval_if.hpp"

#include "mw/attributes/detail/AttributeCompileErrors.h"

#include "util/endianness.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                /*!
                 * \brief Represents an attribute sequence header which simply
                 *  provides a binary representation for the sequence's size,
                 *  i.e. the number of bytes the contained elements (attributes
                 *  in this case) need.
                 *
                 * The binary representation basically consists of one byte. The
                 *  highest bit of this byte signals if another byte will follow.
                 *  If set to 0 the remaining 7 bits represent the actual size. That
                 *  is, in that case sizes up to 127 are possible.
                 *  If set to 1 the following 15 bits (including the 8 bits of the
                 *  next byte) represent the size. In this case sizes up to 32.768
                 *  are possible.
                 *
                 * \tparam setSize The size which should be represented
                 */
                template <uint16_t setSize>
                struct AttributeSetHeader {
                    private:
                        typedef AttributeSetHeader type;

                        // True, if the list header will be written extended, false if the
                        //  header fits one byte
                        static const bool extension = (setSize > 0x7F);

                        // Assert that the sequence size fits the format bounds
                        typedef typename boost::mpl::eval_if_c<
                                                      extension,
                                                      ExtendedSequenceBoundError<setSize>,
                                                      UnextendedSequenceBoundError<setSize>
                                                     >::type assertDummy;

                    public:
                        // The size of the header is 1 if it is not extended and 2 if it
                        //  is extended
                        static const uint8_t size = (extension ? 2 : 1);

                    private:
                        // The member array containing the binary represented list header
                        // (the size is decided by the given sequence size)
                        uint8_t data[size];

                    public:
                        /*!
                         * Creates the binary representation of the size given as the template
                         *  argument in to the member array.
                         */
                        AttributeSetHeader() {
                            // Depending on whether the sequence header is extended either 1
                            //  or 2 two bytes must be written accordingly
                            if (extension) {
                                // Convert the length to network byte order and set the
                                //  extension bit
                                const uint16_t tmpSize = htons(setSize | 0x8000);
                                // Assign the converted value to the array
                                *(reinterpret_cast<uint16_t*> (data)) = tmpSize;
                            } else {
                                // Write the lower 7 bits of the sequence size into the first
                                //  and only byte (The extension flag assures that the given
                                //  sequence size fits 7 bits)
                                data[0] = (setSize & 0x7F);
                            }
                        }
                };

            } // end namespace detail
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _Attribute_Set_Header_
