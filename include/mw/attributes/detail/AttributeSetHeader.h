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
#include "boost/mpl/integral_c.hpp"

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
                        // True, if the set header will be written extended, false if the
                        //  header fits one byte
                        static const bool extension = (setSize > 0x7F);

                        // Assert that the sequence size fits the format bounds
                        typedef typename boost::mpl::eval_if_c<
                                                      extension,
                                                      ExtendedSequenceBoundError<setSize>,
                                                      UnextendedSequenceBoundError<setSize>
                                                     >::type assertDummy;

                    public:
                        // This type
                        typedef AttributeSetHeader type;

                        // Boost integral const conformance (the value type is the type of the
                        //  given size)
                        typedef uint16_t value_type;
                        // Boost integral const conformance (the value of this type is the given
                        //  set size)
                        static const uint16_t value = setSize;
                        // Boost integral const conformance (this type is considered as an
                        //  integral constant)
                        typedef boost::mpl::integral_c_tag tag;

                        // The size of the header is 1 byte if it is not extended and 2 bytes
                        //  if it is extended
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
                            writeSize(setSize, extension);
                        }

                        const bool isExtended() const {
                            return ((data[0] & 0x80) != 0);
                        }

                        const uint16_t get() const {
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
                            // Depending on whether the sequence header is extended either 1
                            //  or 2 two bytes must be written accordingly
                            if (extension) {
                                // Convert the length to network byte order and set the
                                //  extension bit
                                const uint16_t tmpSize = htons(size | 0x8000);
                                // Assign the converted value to the array
                                *(reinterpret_cast<uint16_t*> (data)) = tmpSize;
                            } else {
                                // Write the lower 7 bits of the sequence size into the first
                                //  and only byte (The extension flag assures that the given
                                //  sequence size fits 7 bits)
                                data[0] = (size & 0x7F);
                            }
                        }
                };

            } // end namespace detail
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _Attribute_Set_Header_
