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

#ifndef _First_Byte_Of_Header_
#define _First_Byte_Of_Header_

#include <stdint.h>

#include "mw/attributes/detail/ValueByteCount.h"
#include "mw/attributes/detail/CaseSelector.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                /*!
                 * \brief Represents the first byte of the header of the given attribute.
                 *
                 * As the attribute header in general consists of up to 3 bytes this
                 *  struct is in most cases not sufficient to be used on its own.
                 *
                 * \tparam Attr The attribute to calculate the first header byte for
                 */
                template <typename Attr>
                struct FirstByteOfHeader {
                    private:
                        // The category starts from the 5th bit
                        static const uint8_t catOffset = 4;
                        // The extension bit is always the 4th bit
                        static const uint8_t extBitOffset = 3;
                        // For system attributes the VOL (value-or-length) bit is the 3rd
                        static const uint8_t volBitOffset = 2;

                        // TODO: I will not apply any line breaking to this until anyone tells me how
                        //  to make the building of the values as descriptive as they are now ;)

                        // System
                        //                            CAT                       EXT                     VOL                     LEN
                        static const uint8_t case1 = (Attr::id << catOffset) | (0x0 << extBitOffset) | (0x1 << volBitOffset) | (0x00); // Value fits unextended
                        static const uint8_t case2 = (Attr::id << catOffset) | (0x1 << extBitOffset) | (0x1 << volBitOffset) | (0x00); // Value fits extended
                        static const uint8_t case3 = (Attr::id << catOffset) | (0x0 << extBitOffset) | (0x0 << volBitOffset) | (ValueByteCount<Attr>::value & 0x3); // Length fits unextended
                        static const uint8_t case4 = (Attr::id << catOffset) | (0x1 << extBitOffset) | (0x0 << volBitOffset) | ((ValueByteCount<Attr>::value >> 8) & 0x3); // Length fits extended

                        // Non-system
                        //                           CAT                  EXT                     LEN
                        static const uint8_t case5 = (0xF << catOffset) | (0x0 << extBitOffset) | (ValueByteCount<Attr>::value & 0x7); // Length fits unextended
                        static const uint8_t case6 = (0xF << catOffset) | (0x1 << extBitOffset) | ((ValueByteCount<Attr>::value >> 8) & 0x7); // Length fits extended

                    public:
                        /*!
                         * \brief The first byte of a header for the given attribute.
                         */
                        static const uint8_t value = CaseSelector<Attr, uint8_t, case1, case2, case3, case4, case5, case6>::value;
                };

            } // end namespace detail
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif
