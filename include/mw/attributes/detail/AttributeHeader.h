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

#ifndef _Attribute_Header_h_
#define _Attribute_Header_h_

#include <stdint.h>

#include "util/endianness.h"

#include "mw/attributes/detail/FirstByteOfHeader.h"
#include "mw/attributes/detail/ValueByteCount.h"
#include "mw/attributes/detail/CaseSelector.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                /*!
                 * \brief Represents an actual header of a given attribute in binary
                 *  representation.
                 *
                 * \tparam Attr The attribute for which this header should be instantiated
                 */
                template <typename Attr>
                struct AttributeHeader {
                    private:
                        typedef typename CaseSelector<
                                          uint8_t, 1, 1, 1, 2, (1 + 1), (1 + 1 + 1)
                                         >::type sizeSelector;

                        // The byte count for the attribute header (It could also include a
                        //  part of the attribute's value if the LVS switch is set)
                        static const uint8_t size = sizeSelector::template select_ct<Attr>::value;

                        static const bool res2 = (ValueBitCount<Attr>::value < 9) ? false : true;

                        static const bool useOr = CaseSelector<bool,
                                                               true,
                                                               res2,
                                                               false,
                                                               false,
                                                               false,
                                                               false
                                                              >::template select_ct<Attr>::value;

                        enum {
                            writeLowerLengthBits     = 0x10,
                            writeType                = 0x11,
                            writeLowerLenBitsAndType = 0x12,
                            nothingMore              = 0x13
                        };

                        // We use this to decide in the code below if further bytes
                        //  must be manipulated
                        static const uint8_t toWrite = CaseSelector<uint8_t,
                                                                    nothingMore,
                                                                    nothingMore,
                                                                    nothingMore,
                                                                    writeLowerLengthBits,
                                                                    writeType,
                                                                    writeLowerLenBitsAndType
                                                                   >::template select_ct<Attr>::value;

                        uint8_t data[size];

                    public:
                        typedef AttributeHeader type;

                        /*!
                         * \brief Constructs this specific header, the size is dependent on the
                         *  attribute type given as a template argument
                         */
                        AttributeHeader() {
                            // Either use an OR operation (to not overwrite existing bits already
                            //  set by writing the value) or directly assign the header byte since
                            //  there is no danger overwriting
                            if (useOr) {
                                data[0] |= (FirstByteOfHeader<Attr>::value);
                            } else {
                                data[0] = (FirstByteOfHeader<Attr>::value);
                            }

                            // Handle eventually further written bytes (depending on extension,
                            //  LVS etc.)
                            if (toWrite == writeLowerLengthBits) {
                                data[1] = (ValueByteCount<Attr>::value & 0xFF);
                            } else if (toWrite == writeType) {
                                data[1] = (Attr::id & 0xFF);
                            } else if (toWrite == writeLowerLenBitsAndType) {
                                data[1] = (ValueByteCount<Attr>::value & 0xFF);
                                data[2] = (Attr::id & 0xFF);
                            }
                        }
                };

            } // end namespace detail
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif
