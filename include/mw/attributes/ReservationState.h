/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __RESERVATIONSTATE_H_AD65724EDA0DD5__
#define __RESERVATIONSTATE_H_AD65724EDA0DD5__

#include <stdint.h>

#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/Attribute.h"

#include "mw/attributes/detail/HighDensityIDs.h"

#include "mw/attributes/detail/tags/TagSet.h"
#include "mw/attributes/detail/tags/IsHighDensity.h"
#include "mw/attributes/detail/tags/HasLessThanRelation.h"

namespace famouso {
    namespace mw {
        namespace attributes {

            /*!
             * \brief Defines a reservation state attribute
             *
             * \note  It is used internally for transmitting the reservation
             *        state of a real time producer, i.e. of a publisher
             *        concerning a specific network.
             */
            class ReservationState : public Attribute<
                                            ReservationState,
                                            tags::integral_const_tag,
                                            uint8_t,
                                            3,
                                            detail::HighDensityIDs::reservationStateId,
                                            detail::TagSet<
                                                    detail::IsHighDensity
                                                   >
                                        > {
                public:
                    typedef ReservationState type;
            };

        } /* attributes */
    } /* mw */
} /* famouso */

#endif // __RESERVATIONSTATE_H_AD65724EDA0DD5__

