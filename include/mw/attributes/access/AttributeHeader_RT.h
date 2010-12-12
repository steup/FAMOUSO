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
            namespace access {

                /*!
                 * \brief A structure that provides functionality to access attribute
                 *  header fields at a higher level
                 *
                 * It is considered to be the base type of the attribute type hierarchy
                 *  since basic functionality related to header field access is provided
                 *  here. This type basically provides methods for accessing the ID, the
                 *  extension bit and the encoded value length of the attribute. The
                 *  number of bytes used by the header only can also be calculated.
                 *
                 * It is not constructible itself since it has its constructor declared
                 *  protected. It is assumed that this type is derived by a type for which
                 *  'this' directly points to the first byte of an encoded attribute.
                 */
                struct AttributeHeader_RT {
                    private:
                        typedef famouso::mw::attributes::detail::CaseSelector<
                                 uint8_t, 1, 1, 1, 2, (1 + 1), (1 + 1 + 1)
                                >::type sizeSelector;

                    protected:
                        AttributeHeader_RT() {
                            // Visibility
                        }

                        // Shortcut typedef for the attribute header union type
                        typedef famouso::mw::attributes::detail::AttributeElementHeader elemHeader_t;

                        const elemHeader_t* const asElementHeader() const {
                            return (reinterpret_cast<const elemHeader_t* const>(this));
                        }

                        elemHeader_t* const asElementHeader() {
                            return (reinterpret_cast<elemHeader_t* const>(this));
                        }

                    private:
                        /*!
                         * \brief Read access to the ext-Bit
                         */
                        bool isExtended() const {
                            return (asElementHeader()->extension);
                        }

                    public:
                        /*!
                         * \brief Returns true if the attribute is encoded with high density
                         */
                        bool isHighDensity() const {
                            return (asElementHeader()->isHighDensity());
                        }

                        /*!
                         * \brief Returns the encoded attribute ID no matter if it is encoded
                         *  with high or low density
                         */
                        uint8_t getID() const {
                            if (isHighDensity()) {
                                return (asElementHeader()->category);
                            } else {
                                const uint8_t* const data = reinterpret_cast<const uint8_t* const>(this);

                                return (data[isExtended() ? 2 : 1]);
                            }
                        }

                        /*!
                         * \brief Returns the length of this attribute's encoded value
                         *
                         * The returned length is determined considering all fields
                         *  of the header, that is it always returns the correct
                         *  length for all possible header structures
                         * The returned value should be interpreted as the number
                         *  of bytes needed by the attribute's value, so for the special
                         *  case of a high density attribute with its value encoded
                         *  in the header 0 respective 1 will be returned.
                         *
                         * \return The bytes needed for encoding this attribute's value
                         */
                        uint16_t valueLength() const {
                            if (isHighDensity()) {
                                if (asElementHeader()->lengthValueSwitch) {
                                    // The length is encoded
                                    if (isExtended()) {
                                        // The length is encoded extended, so we reinterpret
                                        //  the masked header as the value length
                                        return (ntohs(*(reinterpret_cast<
                                                         const uint16_t*
                                                        >(this))) & 0x03FF);
                                    } else {
                                        // The length is encoded unextended so we access
                                        //  the encoded length using the union member
                                        return (asElementHeader()->valueOrLength);
                                    }
                                } else {
                                    // Special case that a high density attribute's
                                    //  value is encoded as a part of the header
                                    return (isExtended() ? 1 : 0);
                                }
                            } else {
                                // LD-attributes always have their lengths encoded as
                                //  a part of the header, we only have to check the
                                //  extension bit
                                if (isExtended()) {
                                    return (ntohs(*(reinterpret_cast<
                                                     const uint16_t*
                                                    >(this))) & 0x07FF);
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
                        uint8_t headerLength() const {
                            return (sizeSelector::select_rt(asElementHeader()));
                        }
                };

            } // end namespace access
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif
