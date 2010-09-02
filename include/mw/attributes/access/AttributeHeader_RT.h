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

#ifndef _Attribute_Header_RT_h_
#define _Attribute_Header_RT_h_

#include <stdint.h>

#include "util/endianness.h"

#include "mw/attributes/detail/CaseSelector.h"

#include "mw/attributes/detail/AttributeElementHeader.h"
#include "mw/attributes/detail/HighDensityIDs.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                struct AttributeHeader_RT {
                    private:
                        typedef CaseSelector<
                                 uint8_t, 1, 1, 1, 2, (1 + 1), (1 + 1 + 1)
                                >::type sizeSelector;

                        // Dummy data array
                        uint8_t data[1];

                        const AttributeElementHeader* const asElementHeader() const {
                            return (reinterpret_cast<const AttributeElementHeader* const>(&data[0]));
                        }

                        bool isExtended() const {
                            return (asElementHeader()->extension);
                        }

                    public:
                        bool isHighDensity() const {
                            return (asElementHeader()->isHighDensity());
                        }

                        uint8_t getID() const {
                            if (isHighDensity()) {
                                return (asElementHeader()->category);
                            } else {
                                return (data[isExtended() ? 2 : 1]);
                            }
                        }

                        /*!
                         * \brief Returns the length encoded in this attribute header
                         *
                         * The returned length is determined considering all fields
                         *  of the header, that is it always returns the correct
                         *  length for all possible header structures
                         * The returned value should be interpreted as the number
                         *  of bytes needed by the attribute's value, so the special
                         *  case of a high density attribute with its value encoded
                         *  in the header 0 respective 1 will be returned.
                         *
                         * \return The encoded length field of this attribute header
                         */
                        uint16_t getLength() const {
                            if (isHighDensity()) {
                                if (asElementHeader()->valueOrLengthSwitch) {
                                    // Special case that a high density attribute's
                                    //  value is encoded as a part of the header
                                    return (isExtended() ? 1 : 0);
                                } else {
                                    if (isExtended()) {
                                        return (ntohs(*(reinterpret_cast<
                                                         const uint16_t*
                                                        >(&data[0]))) & 0x03FF);
                                    } else {
                                        return (asElementHeader()->valueOrLength);
                                    }
                                }
                            } else {
                                if (isExtended()) {
                                    return (ntohs(*(reinterpret_cast<
                                                     const uint16_t*
                                                    >(&data[0]))) & 0x07FF);
                                } else {
                                    return (asElementHeader()->length);
                                }
                            }
                        }


                        // Additional helper methods

                        /*!
                         * \brief Runtime accessor for the number of bytes needed by
                         *  this attribute header
                         *
                         * It has the same semantics as type::size
                         *
                         * \return The number of bytes needed by this attribute
                         *  header
                         */
                        uint8_t getSize() const {
                            return (sizeSelector::select_rt(asElementHeader()));
                        }
                };

            } // end namespace detail
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif
