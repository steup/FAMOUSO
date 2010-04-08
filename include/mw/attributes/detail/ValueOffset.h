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

#ifndef _Value_Offset_
#define _Value_Offset_

#include <stdint.h>

#include "ValueByteCount.h"
#include "CaseSelector.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

            /*!
             * \brief Calculates the index of the byte in the binary representation of
             *  the given attribute where the first part of the attribute's value is
             *  contained.
             *
             * \tparam Attr The attribute type of which the value offset should be
             *  calculated
             */
            template <typename Attr>
            struct ValueOffset {
                private:
                    // For the second case (value fits extended) it is necessary to check if
                    //  the value only needs one byte, which would mean that it is simply written
                    //  into the second byte
                    static const uint8_t res2 = (ValueBitCount<Attr>::value < 9) ? 1 : 0;

                public:
                    /*!
                     * The calculated value offset
                     */
                    static const uint8_t value = CaseSelector<Attr, uint8_t, 0, res2, 1, 2, 2, 3>::value;
            };

            } // end namespace detail
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _Value_Offset_
